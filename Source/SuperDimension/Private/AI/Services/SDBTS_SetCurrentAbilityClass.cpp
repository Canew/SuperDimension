


#include "AI/Services/SDBTS_SetCurrentAbilityClass.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

USDBTS_SetCurrentAbilityClass::USDBTS_SetCurrentAbilityClass()
{
	NodeName = TEXT("Set Current Ability Class");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;

	TSubclassOf<UGameplayAbility> SubclassOfGameplayAbility = UGameplayAbility::StaticClass();
	BlackboardKey.AddClassFilter(this, GetSelectedBlackboardKey(), SubclassOfGameplayAbility);
}

void USDBTS_SetCurrentAbilityClass::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn() || !AIController->GetWorld())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return;
	}

	USDAbilitySystemComponent* AbilitySystemComponent = Cast<USDAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (!AbilitySystemComponent)
	{
		return;
	}

	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	for (FGameplayTag AbilityTag : AbilityTags)
	{
		TArray<FGameplayAbilitySpec*> MatchingAbilitySpecs;
		FGameplayTagContainer QueryTags;
		QueryTags.AddTag(AbilityTag);
		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(QueryTags, MatchingAbilitySpecs);
		for (FGameplayAbilitySpec* MatchingAbilitySpec : MatchingAbilitySpecs)
		{
			if (!MatchingAbilitySpec->Ability->CheckCooldown(MatchingAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
			{
				MatchingAbilitySpecs.Remove(MatchingAbilitySpec);
			}
		}

		AbilitySpecs.Append(MatchingAbilitySpecs);
	}

	int32 AbilityIndex = FMath::RandRange(0, AbilitySpecs.Num() - 1);
	if (AbilitySpecs.IsValidIndex(AbilityIndex))
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySpecs[AbilityIndex])
		{
			BlackboardComponent->SetValueAsClass(GetSelectedBlackboardKey(), AbilitySpec->Ability->GetClass());
		}
	}
}

void USDBTS_SetCurrentAbilityClass::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn() || !AIController->GetWorld())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return;
	}

	USDAbilitySystemComponent* AbilitySystemComponent = Cast<USDAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (!AbilitySystemComponent)
	{
		return;
	}

	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	for (FGameplayTag AbilityTag : AbilityTags)
	{
		TArray<FGameplayAbilitySpec*> MatchingAbilitySpecs;
		FGameplayTagContainer QueryTags;
		QueryTags.AddTag(AbilityTag);
		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(QueryTags, MatchingAbilitySpecs);
		for (FGameplayAbilitySpec* MatchingAbilitySpec : MatchingAbilitySpecs)
		{
			if (!MatchingAbilitySpec->Ability->CheckCooldown(MatchingAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
			{
				MatchingAbilitySpecs.Remove(MatchingAbilitySpec);
			}
		}

		AbilitySpecs.Append(MatchingAbilitySpecs);
	}

	int32 AbilityIndex = FMath::RandRange(0, AbilitySpecs.Num() - 1);
	if (AbilitySpecs.IsValidIndex(AbilityIndex))
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySpecs[AbilityIndex])
		{
			BlackboardComponent->SetValueAsClass(GetSelectedBlackboardKey(), AbilitySpec->Ability->GetClass());
		}
	}
}
