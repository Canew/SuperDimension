// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGA_Jump.h"
#include "GameFramework/Character.h"
#include "Components/SDCharacterMovementComponent.h"

bool USDGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor))
	{
		return AvatarCharacter->CanJump();
	}

	return false;
}

void USDGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor))
	{
		AvatarCharacter->Jump();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
