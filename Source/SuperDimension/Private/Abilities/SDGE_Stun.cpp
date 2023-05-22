


#include "Abilities/SDGE_Stun.h"

USDGE_Stun::USDGE_Stun()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Effect.Stun")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun")));
	ApplicationTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.Immune.Stun")));
}
