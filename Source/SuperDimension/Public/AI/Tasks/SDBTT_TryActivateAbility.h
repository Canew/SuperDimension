

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SDBTT_TryActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDBTT_TryActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	USDBTT_TryActivateAbility();

	/** starts this task, should return Succeeded, Failed or InProgress
 *  (use FinishLatentTask() when returning InProgress)
 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	/** helper function: finish latent executing */
	void FinishLatentTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult) const;

protected:
	/** ticks this task
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	virtual void OnAbilityEnded(class UGameplayAbility* Ability);

protected:
	class USDAbilitySystemComponent* OwnerAbilitySystemComponent;
	class UBehaviorTreeComponent* OwnerBehaviorComponent;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AbilityClassKey;

	TSubclassOf<class UGameplayAbility> CurrentAbilityClass;
	struct FGameplayAbilitySpec* CurrentAbilitySpec;

	FDelegateHandle OnAbilityEndedDelegateHandle;
};
