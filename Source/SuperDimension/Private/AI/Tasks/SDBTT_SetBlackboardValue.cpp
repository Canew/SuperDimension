


#include "AI/Tasks/SDBTT_SetBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

USDBTT_SetBlackboardValue::USDBTT_SetBlackboardValue()
{
	NodeName = TEXT("Set Blackboard Value");
	
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USDBTT_SetBlackboardValue, BlackboardKey));
}

EBTNodeResult::Type USDBTT_SetBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (NodeResult == EBTNodeResult::Aborted || NodeResult == EBTNodeResult::Failed)
	{
		return NodeResult;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsBool(GetSelectedBlackboardKey(), bBooleanValue);

	return EBTNodeResult::Succeeded;
}
