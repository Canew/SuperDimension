


#include "Abilities/SDGA_Point.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Weapons/SDExplosion.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void USDGA_Point::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;

	OmenActor = nullptr;
	ExplosionActor = nullptr;
	SpawnLocation = FVector::ZeroVector;

	UAbilityTask_PlayMontageAndWait* PlayRangedMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AbilityMontage, 1.0f, NAME_None, false);
	if (PlayRangedMontageAndWait)
	{
		PlayRangedMontageAndWait->OnCompleted.AddDynamic(this, &USDGA_Point::HandleMontageCompleted);
		PlayRangedMontageAndWait->OnBlendOut.AddDynamic(this, &USDGA_Point::HandleMontageBlendOut);
		PlayRangedMontageAndWait->OnInterrupted.AddDynamic(this, &USDGA_Point::HandleMontageInterrupted);
		PlayRangedMontageAndWait->OnCancelled.AddDynamic(this, &USDGA_Point::HandleMontageCancelled);
		PlayRangedMontageAndWait->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitOmenEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.Omen")));
		if (WaitOmenEvent)
		{
			WaitOmenEvent->EventReceived.AddDynamic(this, &USDGA_Point::HandleOmenEventReceived);
			WaitOmenEvent->ReadyForActivation();
		}

		UAbilityTask_WaitGameplayEvent* WaitExplodevent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.Launch")));
		if (WaitExplodevent)
		{
			WaitExplodevent->EventReceived.AddDynamic(this, &USDGA_Point::HandleLaunchEventReceived);
			WaitExplodevent->ReadyForActivation();
		}
	}
}

void USDGA_Point::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (OmenActor)
	{
		OmenActor->Destroy();
	}
}

void USDGA_Point::HandleMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Point::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Point::HandleMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Point::HandleMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void USDGA_Point::HandleOmenEventReceived(FGameplayEventData Payload)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !OmenClass)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	SpawnLocation = GetSpawnLocation();
	if (!SpawnLocation.IsNearlyZero())
	{
		OmenActor = World->SpawnActorDeferred<AActor>(OmenClass, FTransform(FRotator::ZeroRotator, SpawnLocation), AvatarActor);
		if (OmenActor)
		{
			OmenActor->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));
		}
	}
}

void USDGA_Point::HandleLaunchEventReceived(FGameplayEventData Payload)
{
	if (OmenActor && !OmenActor->IsPendingKill())
	{
		OmenActor->Destroy();
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !ExplosionClass)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	if (!SpawnLocation.IsNearlyZero())
	{
		ExplosionActor = World->SpawnActorDeferred<ASDExplosion>(ExplosionClass, FTransform(FRotator::ZeroRotator, SpawnLocation), AvatarActor);
		if (ExplosionActor)
		{
			ExplosionActor->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));
			ExplosionActor->BeginExplosionCollisionOverlap();
		}
	}
}

FVector USDGA_Point::GetSpawnLocation()
{
	FVector Location = FVector::ZeroVector;

	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!AvatarPawn)
	{
		return Location;
	}

	if (!AvatarPawn->IsPlayerControlled())
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
			{
				if (AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor"))))
				{
					Location = TargetActor->GetActorLocation();
					Location += FVector(FMath::FRand(), FMath::FRand(), FMath::FRand());
				}
			}
		}
	}

	return Location;
}
