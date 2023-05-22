// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "SDCharacter.generated.h"

UCLASS()
class SUPERDIMENSION_API ASDCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASDCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region Landing
public:
	virtual void Landed(const FHitResult& Hit) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Landing")
	float HardLandingVelocityZ;

#pragma endregion


#pragma region Abilities
public:
	// Implementation IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	// Implementation IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// template version of GetAbilitySystemComponent()
	template <typename T>
	T* GetAbilitySystemComponent() const
	{
		return Cast<T>(GetAbilitySystemComponent());
	}

	// Return AttributeSet in PlayerState
	virtual class UAttributeSet* GetAttributeSet() const;

	// template version of GetAttributeSet()
	template <typename T>
	T* GetAttributeSet() const
	{
		return Cast<T>(GetAttributeSet());
	}

protected:
	virtual void InitializeDefaultAbilities();
	virtual void InitializeDefaultAttributes();

	void HandleGameplayEffectExecuteDelegateOnSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class USDGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultAttributes;

#pragma endregion


#pragma region Nickname
public:
	virtual FName GetNickname() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Nickname")
	FName Nickname;


#pragma region Weapon
public:
	virtual void EquipDefaultWeapon();

protected:
	class ASDWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class ASDWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName DefaultWeaponSocketName;

#pragma endregion


#pragma region Perception
protected:
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;

#pragma endregion
	
	
#pragma region Stagger
public:
	virtual void Pushback(AActor* PushInstigator, const FVector& PushBackVelocity, float PushBackStunDuration);

#pragma region 


	UFUNCTION()
	void OnCharacterMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
};
