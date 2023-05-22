// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/SDBTD_CanSeeTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USDBTD_CanSeeTarget::USDBTD_CanSeeTarget()
{
	NodeName = TEXT("Can See Target");

	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
}

bool USDBTD_CanSeeTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn() || !AIController->GetWorld())
	{
		return false;
	}

	APawn* Pawn = AIController->GetPawn();
	UWorld* World = AIController->GetWorld();
	if (AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName)))
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(Pawn);
		if (World->SweepSingleByChannel(HitResult, Pawn->GetActorLocation(), TargetActor->GetActorLocation(), FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SphereRadius), CollisionQueryParams))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void USDBTD_CanSeeTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	TNodeInstanceMemory* DecoratorMemory = CastInstanceNodeMemory<TNodeInstanceMemory>(NodeMemory);
	bool bCondition = CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bCondition != DecoratorMemory->BeforeData)
	{
		DecoratorMemory->BeforeData = bCondition;
		OwnerComp.RequestExecution(this);
	}
}

void USDBTD_CanSeeTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	TNodeInstanceMemory* DecoratorMemory = CastInstanceNodeMemory<TNodeInstanceMemory>(NodeMemory);
	DecoratorMemory->BeforeData = CalculateRawConditionValue(OwnerComp, NodeMemory);
}

uint16 USDBTD_CanSeeTarget::GetInstanceMemorySize() const
{
	return sizeof(TNodeInstanceMemory);
}
