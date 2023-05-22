


#include "Abilities/SDGE_EnableCounter.h"

USDGE_EnableCounter::USDGE_EnableCounter()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.EnableCounter")));
}