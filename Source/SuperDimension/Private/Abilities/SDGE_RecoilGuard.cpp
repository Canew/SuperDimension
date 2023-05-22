


#include "Abilities/SDGE_RecoilGuard.h"

USDGE_RecoilGuard::USDGE_RecoilGuard()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.RecoilGuard")));
}