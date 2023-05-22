

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SDBTT_SetFocus.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDBTT_SetFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	USDBTT_SetFocus();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
