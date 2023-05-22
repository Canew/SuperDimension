

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SDGameplayAbility.h"
#include "SDGA_Point.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGA_Point : public USDGameplayAbility
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
	void HandleOmenEventReceived(FGameplayEventData Payload);
	UFUNCTION()
	void HandleLaunchEventReceived(FGameplayEventData Payload);

	virtual FVector GetSpawnLocation();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	class UAnimMontage* AbilityMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	TSubclassOf<class AActor> OmenClass;
	class AActor* OmenActor;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	TSubclassOf<class ASDExplosion> ExplosionClass;
	class ASDExplosion* ExplosionActor;
	
	FVector SpawnLocation;
};
