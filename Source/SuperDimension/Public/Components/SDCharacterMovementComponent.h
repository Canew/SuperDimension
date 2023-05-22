// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SDCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	USDCharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

	virtual bool IsSprinting() const;
	virtual bool IsImmovable() const;
	virtual bool IsStunned() const;
	virtual bool IsClimbing() const;
	virtual bool IsPassiveAI() const;

	UFUNCTION(BlueprintCallable)
	virtual void EnterWater(float InWaterLevelLocationZ);
	UFUNCTION(BlueprintCallable)
	virtual void ExitWater();

protected:
	virtual void StartSwimming();
	virtual void EndSwimming();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Multiplier")
	float SprintSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Multiplier")
	float PassiveAISpeedMultiplier;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Swimming")
	bool bIsInWater;
	float WaterLevelLocationZ;
};
