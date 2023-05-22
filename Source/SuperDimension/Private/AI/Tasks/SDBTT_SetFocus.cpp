


#include "AI/Tasks/SDBTT_SetFocus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USDBTT_SetFocus::USDBTT_SetFocus()
{
	NodeName = TEXT("Set Focus");

	BlackboardKey.AllowNoneAsValue(true);
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USDBTT_SetFocus, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USDBTT_SetFocus, BlackboardKey));
}

EBTNodeResult::Type USDBTT_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (NodeResult == EBTNodeResult::Aborted || NodeResult == EBTNodeResult::Failed)
	{
		return NodeResult;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	if (BlackboardKey.IsNone())
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Succeeded;
	}

	UObject* SelectedBlackboardObject = BlackboardComponent->GetValueAsObject(GetSelectedBlackboardKey());
	if (SelectedBlackboardObject)
	{
		if (AActor* SelectedBlackboardActor = Cast<AActor>(SelectedBlackboardObject))
		{
			AIController->SetFocus(SelectedBlackboardActor, EAIFocusPriority::Gameplay);
		}
	}
	else
	{
		const FVector& SelectedBlackboardVector = BlackboardComponent->GetValueAsVector(GetSelectedBlackboardKey());
		if (!SelectedBlackboardVector.IsNearlyZero())
		{
			AIController->SetFocalPoint(SelectedBlackboardVector, EAIFocusPriority::Gameplay);
		}
	}

	return EBTNodeResult::Succeeded;
}
