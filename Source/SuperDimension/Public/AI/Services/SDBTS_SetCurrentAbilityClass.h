

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "GameplayTagContainer.h"
#include "SDBTS_SetCurrentAbilityClass.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDBTS_SetCurrentAbilityClass : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	USDBTS_SetCurrentAbilityClass();
	
protected:
	/** called when auxiliary node becomes active
 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	/** update next tick interval
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Tag")
	FGameplayTagContainer AbilityTags;
};
