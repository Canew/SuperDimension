// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "SDBTD_ForwardConeCheck.generated.h"

/**
 *
 */
UCLASS()
class SUPERDIMENSION_API USDBTD_ForwardConeCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	USDBTD_ForwardConeCheck();

protected:
	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual float CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Forward Cone Check")
	float ConeHalfAngle;
};
