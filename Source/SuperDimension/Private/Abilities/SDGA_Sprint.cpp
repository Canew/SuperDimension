// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGA_Sprint.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/SDGT_WaitSpeedChange.h"

USDGA_Sprint::USDGA_Sprint()
{
	WaitSpeedChangeDelay = 0.5f;
}

bool USDGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor))
	{
		return !AvatarCharacter->GetCharacterMovement()->IsFalling();
	}

	return false;
}

void USDGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}
	
	AvatarActor->GetWorldTimerManager().SetTimer(WaitSpeedChangeDelayTimerHandle, this, &USDGA_Sprint::BeginWaitSpeedChangeTask, WaitSpeedChangeDelay, false);
}

void USDGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USDGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void USDGA_Sprint::BeginWaitSpeedChangeTask()
{
	if (USDGT_WaitSpeedChange* WaitSpeedChangeEvent = USDGT_WaitSpeedChange::CreateWaitSpeedChangeEvent(this, KINDA_SMALL_NUMBER))
	{
		WaitSpeedChangeEvent->OnChange.AddDynamic(this, &USDGA_Sprint::HandleSpeedChanged);
		WaitSpeedChangeEvent->ReadyForActivation();
	}
	else
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}

void USDGA_Sprint::HandleSpeedChanged()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
