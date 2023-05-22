

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SDGameplayAbility.h"
#include "SDGA_Guard.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGA_Guard : public USDGameplayAbility
{
	GENERATED_BODY()
	
public:
	USDGA_Guard();

protected:
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	virtual void PlayGuardActivationParticle();
	virtual void PlayGuardTriggerParticle();

	UFUNCTION()
	void HandleMontageCompleted();
	UFUNCTION()
	void HandleMontageBlendOut();
	UFUNCTION()
	void HandleMontageInterrupted();
	UFUNCTION()
	void HandleMontageCancelled();

	virtual void HandleConfirmAbilityEventReceived(FGameplayEventData Payload) override;

	UFUNCTION()
	void HandleGameplayEffectBlocked(FGameplayEffectSpecHandle BlockedSpec, FActiveGameplayEffectHandle ImmunityGameplayEffectHandle);

	virtual bool IsInResponseAngle(AActor* Center, AActor* Other);

	virtual void ApplySuspendedGameplayEffects();

	virtual void SetPhysicalMaterial();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	class UAnimMontage* GuardMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ResponseGameplayEffectTag;

	UPROPERTY(EditDefaultsOnly)
	float ResponseAngle;

	UPROPERTY(EditDefaultsOnly, Category = "Trigger")
	FGameplayTag TriggerEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Immune")
	TArray<TSubclassOf<UGameplayEffect>> ImmuneGameplayEffectClasses;
	TArray<FActiveGameplayEffectHandle> ImmuneGameplayEffectHandles;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	FName ParticleAttachName;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	FVector LocationOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	FRotator RotationOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	UParticleSystem* GuardActivationParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	UParticleSystem* GuardTriggerParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	class USoundBase* GuardTriggerSound;

	// Delay that apply to self if guard successfully
	UPROPERTY(EditDefaultsOnly, Category = "Counter")
	float GuardRecoilTime;
	FTimerHandle GuardRecoilTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Counter")
	float CounterTimeLimit;

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	class UPhysicalMaterial* BodyPhysicalMaterial;

	UPROPERTY()
	UParticleSystemComponent* GuardActivationParticleComponent;
	UPROPERTY()
	UParticleSystemComponent* GuardTriggerParticleComponent;

	UPROPERTY()
	FActiveGameplayEffectHandle RecoilGuardEffectHandle;
	UPROPERTY()
	FActiveGameplayEffectHandle CounterEffectHandle;

	bool bGuardSuccessfully;
	bool bEncounterResponseGameplayEffectTag;
	bool bAlreadySuspendedGameplayEffectApplied;
	TArray<TPair<class UAbilitySystemComponent*, FGameplayEffectSpecHandle>> SuspendedGameplayEffectSpecHandles;
};
