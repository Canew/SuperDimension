// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/SDBTD_ForwardConeCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USDBTD_ForwardConeCheck::USDBTD_ForwardConeCheck()
{
	NodeName = TEXT("Forward Cone Check");
}

bool USDBTD_ForwardConeCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn())
	{
		return false;
	}

	APawn* Pawn = AIController->GetPawn();

	if (AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName)))
	{
		float AngleDifferenceRadian = FMath::Acos(CalculateAngleDifferenceDot(Pawn->GetActorForwardVector(), (TargetActor->GetActorLocation() - Pawn->GetActorLocation())));
		float AngleDifferenceDegree = FMath::RadiansToDegrees(AngleDifferenceRadian);

		return AngleDifferenceDegree <= ConeHalfAngle;
	}

	return false;
}

float USDBTD_ForwardConeCheck::CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB) const
{
	return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero()) ? 1.0f : VectorA.CosineAngle2D(VectorB);
}