

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SDGameplayAbility.h"
#include "SDGA_Climb.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGA_Climb : public USDGameplayAbility
{
	GENERATED_BODY()
	
public:
	USDGA_Climb();

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	class UAnimMontage* ClimbMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	float MaxClimbHeight;
	float TraceDistance;
	float TraceHeight;

	float ClimbingHalfHeight;
	float OriginalHalfHeight;
};
