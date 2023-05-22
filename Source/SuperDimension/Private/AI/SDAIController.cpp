// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SDAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "SDPlayerController.h"

ASDAIController::ASDAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	bWantsPlayerState = true;

	// Setup the perception component
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 2000.0f;
	SightSenseConfig->LoseSightRadius = 2000.0f;
	SightSenseConfig->SetMaxAge(8.0f);
	SightSenseConfig->PeripheralVisionAngleDegrees = 180.0f;
	GetPerceptionComponent()->ConfigureSense(*SightSenseConfig);

	DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	DamageSenseConfig->SetMaxAge(8.0f);
	GetPerceptionComponent()->ConfigureSense(*DamageSenseConfig);

	HearingSenseConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingSenseConfig"));
	HearingSenseConfig->HearingRange = 350.0f;
	HearingSenseConfig->SetMaxAge(8.0f);
	GetPerceptionComponent()->ConfigureSense(*HearingSenseConfig);

	GetPerceptionComponent()->SetDominantSense(SightSenseConfig->GetSenseImplementation());

	TargetActorKeyName = FName(TEXT("TargetActor"));
	HomeLocationKeyName = FName(TEXT("HomeLocation"));

	SmoothFocusInterpSpeed = 30.0f;
}

void ASDAIController::BeginPlay()
{
	Super::BeginPlay();

	RunAI();

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ASDAIController::OnTargetPerceptionUpdated);

	// Generic team
	SetGenericTeamId(2);
	UAIPerceptionSystem::GetCurrent(GetWorld())->UpdateListener(*GetAIPerceptionComponent());

	InitializeHomeLocation();
}

void ASDAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, false);

	// Smooth and change the pawn rotation
	if (bUpdatePawn)
	{
		APawn* const MyPawn = GetPawn();
		const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

		FRotator SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(MyPawn->GetActorRotation(), ControlRotation, DeltaTime, SmoothFocusInterpSpeed);
		if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
		{
			MyPawn->FaceRotation(SmoothTargetRotation, DeltaTime);
		}
	}
}

void ASDAIController::RunAI()
{
	if (BlackboardData && BehaviorTree)
	{
		if (UseBlackboard(BlackboardData, Blackboard))
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void ASDAIController::RestartAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->RestartTree();
	}
}

void ASDAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}
}

void ASDAIController::PredictBeingHit()
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("bPredictBeingHit"), true);
		GetWorldTimerManager().SetTimer(ResetPredictBeingHitValueTimerHandle, FTimerDelegate::CreateLambda(
			[this]() -> void
			{
				UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
				if (BlackboardComponent)
				{
					GetBlackboardComponent()->SetValueAsBool(TEXT("bPredictBeingHit"), false);
				}
			}), 0.1f, false);
	}
}

bool ASDAIController::IsAggressive() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		return GetBlackboardComponent()->GetValueAsBool(TEXT("bAggressive"));
	}

	return false;
}

void ASDAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (GetPawn() == nullptr)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		SetVisibilityPawnHealthToTarget(Actor, true);
		TargetClosestHostileActor();
	}
	else
	{
		TargetClosestHostileActor();

		float TargetExpirationTime;
		if (SightSenseConfig)
		{
			TargetExpirationTime = SightSenseConfig->GetMaxAge();
		}
		else
		{
			TargetExpirationTime = 5.0f;
		}

		FTimerDelegate TargetExpirationTimerDelegate;
		TargetExpirationTimerDelegate.BindUObject(this, &ASDAIController::ExpireTarget, Actor, Stimulus);
		GetWorldTimerManager().SetTimer(TargetExpirationTimerHandle, TargetExpirationTimerDelegate, TargetExpirationTime, false);
	}
}

void ASDAIController::InitializeHomeLocation()
{
	if (Blackboard)
	{
		if (APawn* MyPawn = GetPawn())
		{
			Blackboard->SetValueAsVector(HomeLocationKeyName, MyPawn->GetActorLocation());
		}
	}
}

void ASDAIController::TargetClosestHostileActor()
{
	AActor* ClosestActor = nullptr;
	TArray<AActor*> PerceivedHostileActors;
	GetAIPerceptionComponent()->GetPerceivedHostileActors(PerceivedHostileActors);
	for (AActor* PerceivedHostileActor : PerceivedHostileActors)
	{
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(PerceivedHostileActor->GetRootComponent());
		if (!RootPrimitiveComponent || RootPrimitiveComponent->GetCollisionProfileName() == TEXT("NoCollision"))
		{
			continue;
		}

		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PerceivedHostileActor);
		if (!AbilitySystemComponent || AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Cloaking"))))
		{
			continue;
		}

		if (!ClosestActor)
		{
			ClosestActor = PerceivedHostileActor;
			continue;
		}

		if (GetPawn()->GetDistanceTo(ClosestActor) > GetPawn()->GetDistanceTo(PerceivedHostileActor))
		{
			ClosestActor = PerceivedHostileActor;
		}
	}

	if (Blackboard)
	{
		Blackboard->SetValueAsObject(TargetActorKeyName, ClosestActor);
	}
}

void ASDAIController::ExpireTarget(AActor* Actor, FAIStimulus Stimulus)
{
	SetVisibilityPawnHealthToTarget(Actor, false);
	TargetClosestHostileActor();
}

void ASDAIController::SetVisibilityPawnHealthToTarget(AActor* Target, bool bVisible)
{
	if (APawn* TargetPawn = Cast<APawn>(Target))
	{
		if (ASDPlayerController* PlayerController = Cast<ASDPlayerController>(TargetPawn->GetController()))
		{
			bVisible ? PlayerController->SetHUDBossActor(GetPawn()) : PlayerController->SetHUDBossActor(nullptr);
		}
	}
}
