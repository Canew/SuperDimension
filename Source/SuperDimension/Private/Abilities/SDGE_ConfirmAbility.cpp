


#include "Abilities/SDGE_ConfirmAbility.h"

USDGE_ConfirmAbility::USDGE_ConfirmAbility()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.ConfirmAbility")));

}
