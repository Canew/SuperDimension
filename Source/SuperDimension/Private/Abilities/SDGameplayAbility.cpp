// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/SDGE_Immovable.h"
#include "Abilities/SDGE_EnableCombo.h"
#include "Abilities/SDGE_ConfirmAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "SDPlayerController.h"

USDGameplayAbility::USDGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateActivationOwnedTags = true;
	bActivateOnlyInputStart = true;
	MinActivatableRadius = 0.0f;
	MaxActivatableRadius = 0.0f;
	ExpectedEffectiveRange = 0.0f;
	bImmovableWhenAbilityActivated = false;
	bImmovableWhenAbilityConfirmed = false;
	bReceiveEndAbilityEvent = true;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun")));
}

bool USDGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!IsInActivatableRadiusAI(ActorInfo))
	{
		return false;
	}

	return true;
}

void USDGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (bReplicateActivationOwnedTags)
	{
		for (FGameplayTag ActivationOwnedTag : ActivationOwnedTags)
		{
			if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
			{
				AbilitySystemComponent->AddMinimalReplicationGameplayTag(ActivationOwnedTag);
			}
		}
	}
}

void USDGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;

	if (bImmovableWhenAbilityActivated)
	{
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FGameplayEffectSpecHandle ImmovableEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(USDGE_Immovable::StaticClass(), UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
		ImmovableActiveGameplayEffectHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ImmovableEffectSpecHandle.Data);
	}

	UAbilityTask_WaitGameplayEvent* WaitConfirmAbilityEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.ConfirmAbility")));
	if (WaitConfirmAbilityEvent)
	{
		WaitConfirmAbilityEvent->EventReceived.AddDynamic(this, &USDGameplayAbility::HandleConfirmAbilityEventReceived);
		WaitConfirmAbilityEvent->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* WaitEnableComboEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.EnableCombo")));
	if (WaitEnableComboEvent)
	{
		WaitEnableComboEvent->EventReceived.AddDynamic(this, &USDGameplayAbility::HandleEnableComboEventReceived);
		WaitEnableComboEvent->ReadyForActivation();
	}

	if (bReceiveEndAbilityEvent)
	{
		UAbilityTask_WaitGameplayEvent* WaitEndAbilityEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.EndAbility")));
		if (WaitEndAbilityEvent)
		{
			WaitEndAbilityEvent->EventReceived.AddDynamic(this, &USDGameplayAbility::HandleEndAbilityEventReceived);
			WaitEndAbilityEvent->ReadyForActivation();
		}
	}
}

void USDGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (bReplicateActivationOwnedTags)
		{
			for (FGameplayTag ActivationOwnedTag : ActivationOwnedTags)
			{
				AbilitySystemComponent->RemoveMinimalReplicationGameplayTag(ActivationOwnedTag);

				FGameplayTagContainer EnableComboTagContainer;
				EnableComboTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.EnableCombo")));
				AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(EnableComboTagContainer);
			}
		}

		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ImmovableActiveGameplayEffectHandle);
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ConfirmAbilityActiveGameplayEffectHandle);
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(EnableComboActiveGameplayEffectHandle);
	}

	if (APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (ASDPlayerController* PlayerController = Cast<ASDPlayerController>(AvatarPawn->GetController()))
		{
			PlayerController->InjectRecentInputForAction();
		}
	}
}

void USDGameplayAbility::HandleConfirmAbilityEventReceived(FGameplayEventData Payload)
{
	if (!bImmovableWhenAbilityActivated && bImmovableWhenAbilityConfirmed)
	{
		FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FGameplayEffectSpecHandle ImmovableEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(USDGE_Immovable::StaticClass(), UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
		ImmovableActiveGameplayEffectHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ImmovableEffectSpecHandle.Data);

		FGameplayEffectSpecHandle ConfirmAbilityEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(USDGE_ConfirmAbility::StaticClass(), UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
		ConfirmAbilityActiveGameplayEffectHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ConfirmAbilityEffectSpecHandle.Data);
	}
}

void USDGameplayAbility::HandleEndAbilityEventReceived(FGameplayEventData Payload)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGameplayAbility::HandleEnableComboEventReceived(FGameplayEventData Payload)
{
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle EnableComboffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(USDGE_EnableCombo::StaticClass(), UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
	EnableComboActiveGameplayEffectHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*EnableComboffectSpecHandle.Data);

	if (APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		if (ASDPlayerController* PlayerController = Cast<ASDPlayerController>(AvatarPawn->GetController()))
		{
			PlayerController->InjectRecentInputForAction();
		}
	}
}

bool USDGameplayAbility::IsInActivatableRadiusAI(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor))
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* UBlackboardComponent = AIController->GetBlackboardComponent())
			{
				AActor* TargetActor = Cast<AActor>(UBlackboardComponent->GetValueAsObject(TEXT("TargetActor")));
				float DistanceToTargetActor = AvatarPawn->GetDistanceTo(TargetActor);
				if ((MaxActivatableRadius < KINDA_SMALL_NUMBER || DistanceToTargetActor <= MaxActivatableRadius) && DistanceToTargetActor >= MinActivatableRadius)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}

