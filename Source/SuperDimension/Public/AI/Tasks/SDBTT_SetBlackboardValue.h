

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SDBTT_SetBlackboardValue.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDBTT_SetBlackboardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	USDBTT_SetBlackboardValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, Category = "Value")
	bool bBooleanValue;
};
