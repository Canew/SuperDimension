


#include "AI/Tasks/SDBTT_TryActivateAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

USDBTT_TryActivateAbility::USDBTT_TryActivateAbility()
{
	NodeName = FString(TEXT("Try Activate Ability"));
	bNotifyTick = true;
	bCreateNodeInstance = true;

	TSubclassOf<UGameplayAbility> SubclassOfGameplayAbility = UGameplayAbility::StaticClass();
	AbilityClassKey.AddClassFilter(this, AbilityClassKey.SelectedKeyName, SubclassOfGameplayAbility);
}

EBTNodeResult::Type USDBTT_TryActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerBehaviorComponent = &OwnerComp;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return EBTNodeResult::Failed;
	}

	OwnerAbilitySystemComponent = Cast<USDAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (!OwnerAbilitySystemComponent)
	{
		return EBTNodeResult::Failed;
	}

	CurrentAbilityClass = AIController->GetBlackboardComponent()->GetValueAsClass(AbilityClassKey.SelectedKeyName);
	CurrentAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(CurrentAbilityClass, true);
	CurrentAbilitySpec->SourceObject = AIController->GetPawn();
	if (CurrentAbilitySpec)
	{
		if (OwnerAbilitySystemComponent->TryActivateAbility(CurrentAbilitySpec->Handle))
		{
			if (!OwnerAbilitySystemComponent->AbilityEndedCallbacks.IsBoundToObject(this))
			{
				OnAbilityEndedDelegateHandle = OwnerAbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &USDBTT_TryActivateAbility::OnAbilityEnded);
			}

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void USDBTT_TryActivateAbility::FinishLatentTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult) const
{
	Super::FinishLatentTask(OwnerComp, TaskResult);

	if (OnAbilityEndedDelegateHandle.IsValid())
	{
		OwnerAbilitySystemComponent->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
	}
}

void USDBTT_TryActivateAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (FGameplayAbilitySpec* GameplayAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromHandle(CurrentAbilitySpec->Handle))
	{
		if (!GameplayAbilitySpec->IsActive())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
}

void USDBTT_TryActivateAbility::OnAbilityEnded(UGameplayAbility* Ability)
{
	if (!Ability || !CurrentAbilitySpec)
	{
		return;
	}

	if (Ability->GetClass() == CurrentAbilityClass)
	{
		FinishLatentTask(*OwnerBehaviorComponent, EBTNodeResult::Succeeded);
		return;
	}
}

