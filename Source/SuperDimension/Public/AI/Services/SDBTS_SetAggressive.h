

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SDBTS_SetAggressive.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDBTS_SetAggressive : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	USDBTS_SetAggressive();

protected:
	/** update next tick interval
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// Probability to set bAggressive true
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float AggressiveProbability;
};
