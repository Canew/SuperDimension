


#include "Abilities/SDGE_Immovable.h"

USDGE_Immovable::USDGE_Immovable()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Effect.Immovable")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Immovable")));
	ApplicationTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.Immune.Immovable")));
}