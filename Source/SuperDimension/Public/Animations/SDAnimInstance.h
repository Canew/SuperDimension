// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SDAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	USDAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	//UFUNCTION()
	//virtual void StartHitReact(EPADirection NewHitReactDirection);

private:
	/** whether character is falling*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsFalling;

	/** whether character is flying */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsFlying;

	/** whether character is swimming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsSwimming;

	/** Velocity size of character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Speed;

	/** Delta that character rotation and control rotation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Roll;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Pitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Yaw;

	/** Delta that current yaw and yaw at last frame */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float YawDelta;

	/** Character's move direction */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float MoveDirection;

	/** Value for proper tilt of character */
	float LeanIntensityScale;
	float LeanInterpolationSpeed;

	/** Store FRotator at last tick */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FRotator RotationLastTick;

	/** Whether character's velocity is being increased */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsAccelerating;

	/** Whether character is sprinting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsSprinting;

	/** Whether character is stunned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsStunned;

	/** Whether character is dead */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsDead;

	/** if character should play full body animation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bFullBody;
};
