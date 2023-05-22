// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/SDBTD_ActiveAbility.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

USDBTD_ActiveAbility::USDBTD_ActiveAbility()
{
	NodeName = TEXT("Active Ability");
}

bool USDBTD_ActiveAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return false;
	}

	USDAbilitySystemComponent* AbilitySystemComponent = Cast<USDAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (!AbilitySystemComponent)
	{
		return false;
	}

	if (FGameplayAbilitySpec* GameplayAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass, bFindChildClass))
	{
		return GameplayAbilitySpec->IsActive();
	}

	return false;
}
