

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SDBTD_CanActivateAbility.generated.h"

struct FBTDecorator_CanActivateAbility_Memory
{
	bool BeforeData;
};

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDBTD_CanActivateAbility : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USDBTD_CanActivateAbility();

protected:
	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/** tick function
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	/** called when auxiliary node becomes active
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	uint16 GetInstanceMemorySize() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AbilityClassKey;
};
