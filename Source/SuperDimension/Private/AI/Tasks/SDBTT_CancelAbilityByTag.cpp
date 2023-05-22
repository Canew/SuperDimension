// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SDBTT_CancelAbilityByTag.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

USDBTT_CancelAbilityByTag::USDBTT_CancelAbilityByTag()
{
	NodeName = FString(TEXT("Cancel Ability By Tag"));
}

EBTNodeResult::Type USDBTT_CancelAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	OwnerAbilitySystemComponent->CancelAbilities(&WithTags, &WithoutTags);

	return EBTNodeResult::Succeeded;
}
