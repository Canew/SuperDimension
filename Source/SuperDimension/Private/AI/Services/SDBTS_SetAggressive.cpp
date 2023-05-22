


#include "AI/Services/SDBTS_SetAggressive.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USDBTS_SetAggressive::USDBTS_SetAggressive()
{
	NodeName = TEXT("Set Aggressive");

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USDBTS_SetAggressive, BlackboardKey));
}

void USDBTS_SetAggressive::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	if (AggressiveProbability == 0.0f)
	{
		BlackboardComponent->SetValueAsBool(GetSelectedBlackboardKey(), false);
	}

	float RandomValue = FMath::FRand();
	if (RandomValue <= AggressiveProbability)
	{
		BlackboardComponent->SetValueAsBool(GetSelectedBlackboardKey(), true);
	}
	else
	{
		BlackboardComponent->SetValueAsBool(GetSelectedBlackboardKey(), false);
	}
}
