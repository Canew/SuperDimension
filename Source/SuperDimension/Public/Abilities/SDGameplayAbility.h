// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SDGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USDGameplayAbility();

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	
protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:
	UFUNCTION()
	virtual void HandleConfirmAbilityEventReceived(FGameplayEventData Payload);
	UFUNCTION()
	virtual void HandleEndAbilityEventReceived(FGameplayEventData Payload);
	UFUNCTION()
	virtual void HandleEnableComboEventReceived(FGameplayEventData Payload);

	// Return true if ai's TargetActor is closer than ExpectedEffectiveRadius
	virtual bool IsInActivatableRadiusAI(const FGameplayAbilityActorInfo* ActorInfo) const;
		
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* AbilityInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bActivateOnlyInputStart;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	bool bReplicateActivationOwnedTags;

	// Activatable minimum radius used in ai, ActivatableMinRadius == 0.0f means no min limit
	UPROPERTY(EditDefaultsOnly, Category = "Range")
	float MinActivatableRadius;

	// Activatable maximum radius used in ai, MaxActivatableRadius == 0.0f means infinite
	UPROPERTY(EditDefaultsOnly, Category = "Range")
	float MaxActivatableRadius;

	// Expected ability range
	UPROPERTY(EditDefaultsOnly, Category = "Range")
	float ExpectedEffectiveRange;

	UPROPERTY(EditDefaultsOnly, Category = "Restraint")
	bool bImmovableWhenAbilityActivated;
	UPROPERTY(EditDefaultsOnly, Category = "Restraint", Meta = (EditCondition = "!bImmovableWhenAbilityActivated"))
	bool bImmovableWhenAbilityConfirmed;

	UPROPERTY(EditDefaultsOnly, Category = "Event")
	bool bReceiveEndAbilityEvent;

	UPROPERTY()
	FActiveGameplayEffectHandle ImmovableActiveGameplayEffectHandle;
	UPROPERTY()
	FActiveGameplayEffectHandle ConfirmAbilityActiveGameplayEffectHandle;
	UPROPERTY()
	FActiveGameplayEffectHandle EnableComboActiveGameplayEffectHandle;
};
