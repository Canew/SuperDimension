

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SDGameplayAbility.h"
#include "SDGA_Ranged.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGA_Ranged : public USDGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION()
	void HandleMontageCompleted();
	UFUNCTION()
	void HandleMontageBlendOut();
	UFUNCTION()
	void HandleMontageInterrupted();
	UFUNCTION()
	void HandleMontageCancelled();

	UFUNCTION()
	void HandleLaunchEventReceived(FGameplayEventData Payload);

	FVector GetProjectileDirection() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	class UAnimMontage* RangedMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ASDProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName MuzzleSocketName;
};
