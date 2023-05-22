// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "SDBTD_CanSeeTarget.generated.h"

struct FBTDecorator_CanSeeTarget_Memory
{
	bool BeforeData;
};

typedef FBTDecorator_CanSeeTarget_Memory TNodeInstanceMemory;

/**
 *
 */
UCLASS()
class SUPERDIMENSION_API USDBTD_CanSeeTarget : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	USDBTD_CanSeeTarget();

protected:
	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	/** tick function
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	/** called when auxiliary node becomes active
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	uint16 GetInstanceMemorySize() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, Category = "Collision")
	float SphereRadius;
};
