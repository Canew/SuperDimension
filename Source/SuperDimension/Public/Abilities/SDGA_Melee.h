// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SDGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SDGA_Melee.generated.h"

UENUM()
enum class EMeleeCollisionShape : uint8
{
	Box,
	Sphere,
	Capsule
};

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGA_Melee : public USDGameplayAbility
{
	GENERATED_BODY()

public:
	USDGA_Melee();

protected:
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	/** Get weapon attached to socket on avatar actor  */
	virtual class ASDWeapon* GetAvatarWeapon(FName AttachSocketName);

	// Handler function to receive ability task event

	void HandleConfirmAbilityEventReceived(FGameplayEventData Payload) override;

	UFUNCTION()
	void HandleMontageCompleted();
	UFUNCTION()
	void HandleMontageBlendOut();
	UFUNCTION()
	void HandleMontageInterrupted();
	UFUNCTION()
	void HandleMontageCancelled();

	UFUNCTION()
	void HandleBeginOverlapEventReceived(FGameplayEventData Payload);
	void BeginWaitOverlapEvent();

	UFUNCTION()
	void HandleEndOverlapEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void HandleMeleeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	class UAnimMontage* MeleeMontage;

	UPROPERTY(EditDefaultsOnly)
	FName MeleeAttachSocketName;

	UPROPERTY(EditDefaultsOnly)
	FName MeleeCollisionProfileName;

	FGameplayTag BeginOverlapTag;
	FGameplayTag EndOverlapTag;

	UPROPERTY(EditDefaultsOnly)
	EMeleeCollisionShape MeleeCollisionShapeType;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "MeleeCollisionShapeType == EMeleeCollisionShape::Box", EditConditionHides))
	FVector BoxHalfExtent;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "MeleeCollisionShapeType == EMeleeCollisionShape::Sphere", EditConditionHides))
	float SphereRadius;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "MeleeCollisionShapeType == EMeleeCollisionShape::Capsule", EditConditionHides))
	float CapsuleHalfHeight;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "MeleeCollisionShapeType == EMeleeCollisionShape::Capsule", EditConditionHides))
	float CapsuleRadius;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	TSubclassOf<class UGameplayEffect> MeleeGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag MeleeGameplayCueTag;

	TWeakObjectPtr<class ASDWeapon> AvatarWeapon;

	class USDGT_WaitOverlapEvent* WaitMeleeOverlapEvent;

	TArray<AActor*> AffectedActors;

#pragma region HitStun
protected:
	// The force to push hit actor by this ability, don't push if (PushBackForce == 0.0f)
	UPROPERTY(EditDefaultsOnly, Category = "HitStun")
	float PushBackForce;

	UPROPERTY(EditDefaultsOnly, Category = "HitStun")
	float PushBackStunDuration;

#pragma endregion
};
