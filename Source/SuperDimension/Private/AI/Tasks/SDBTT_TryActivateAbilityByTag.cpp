// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SDBTT_TryActivateAbilityByTag.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

USDBTT_TryActivateAbilityByTag::USDBTT_TryActivateAbilityByTag()
{
	NodeName = FString(TEXT("Try Activate Ability By Tag"));
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USDBTT_TryActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	TArray<FGameplayAbilitySpec*> GameplayAbilitySpecs;
	OwnerAbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTags, GameplayAbilitySpecs);
	int32 AbilityIndex = FMath::RandRange(0, GameplayAbilitySpecs.Num() - 1);
	if (!GameplayAbilitySpecs.IsValidIndex(AbilityIndex))
	{
		return EBTNodeResult::Failed;
	}

	CurrentAbilitySpec = GameplayAbilitySpecs[AbilityIndex];
	CurrentAbilitySpec->SourceObject = AIController->GetPawn();
	if (CurrentAbilitySpec)
	{
		if (OwnerAbilitySystemComponent->TryActivateAbility(CurrentAbilitySpec->Handle))
		{
			if (!OwnerAbilitySystemComponent->AbilityEndedCallbacks.IsBoundToObject(this))
			{
				OnAbilityEndedDelegateHandle = OwnerAbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &USDBTT_TryActivateAbilityByTag::OnAbilityEnded);
			}

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void USDBTT_TryActivateAbilityByTag::FinishLatentTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult) const
{
	Super::FinishLatentTask(OwnerComp, TaskResult);

	if (OnAbilityEndedDelegateHandle.IsValid())
	{
		OwnerAbilitySystemComponent->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
	}
}

void USDBTT_TryActivateAbilityByTag::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

void USDBTT_TryActivateAbilityByTag::OnAbilityEnded(UGameplayAbility* Ability)
{
	if (!Ability)
	{
		return;
	}

	if (Ability->GetClass() == CurrentAbilitySpec->Ability->GetClass())
	{
		FinishLatentTask(*OwnerBehaviorComponent, EBTNodeResult::Succeeded);
		return;
	}
}
