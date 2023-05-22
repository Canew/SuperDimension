// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/SDAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SDCharacterMovementComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Kismet/KismetMathLibrary.h"

USDAnimInstance::USDAnimInstance()
{
	LeanIntensityScale = 7.0f;
	LeanInterpolationSpeed = 6.0f;
}

void USDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


}

void USDAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void USDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		bIsFalling = Pawn->GetMovementComponent()->IsFalling();
		bIsFlying = Pawn->GetMovementComponent()->IsFlying();
		bIsSwimming = Pawn->GetMovementComponent()->IsSwimming();
		if (bIsFalling)
		{
			SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
		}
		else
		{
			SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
		}
		Speed = Pawn->GetVelocity().Size();

		FRotator RotatorDeltaActorAndControl = UKismetMathLibrary::NormalizedDeltaRotator(Pawn->GetBaseAimRotation(), Pawn->GetActorRotation());
		Roll = RotatorDeltaActorAndControl.Roll;
		Pitch = RotatorDeltaActorAndControl.Pitch;
		Yaw = RotatorDeltaActorAndControl.Yaw;

		FRotator RotatorDeltaActorAndLastTick = UKismetMathLibrary::NormalizedDeltaRotator(RotationLastTick, Pawn->GetActorRotation());
		YawDelta = FMath::FInterpTo(YawDelta, RotatorDeltaActorAndLastTick.Yaw / DeltaSeconds / LeanIntensityScale, DeltaSeconds, LeanInterpolationSpeed);
		RotationLastTick = Pawn->GetActorRotation();

		FVector ForwardVector = FVector(Pawn->GetActorForwardVector().X, Pawn->GetActorForwardVector().Y, 0.0f);
		FVector MoveVelocity = FVector(Pawn->GetVelocity().X, Pawn->GetVelocity().Y, 0.0f);
		ForwardVector.Normalize();
		MoveVelocity.Normalize();
		
		if (Speed > 0.0f)
		{
			MoveDirection = FMath::RadiansToDegrees(FMath::Acos(ForwardVector | MoveVelocity));
		}

		float DistanceToFrontBackPlane = FVector::PointPlaneDist(Pawn->GetActorLocation() + Pawn->GetVelocity(), Pawn->GetActorLocation(), Pawn->GetActorRightVector());
		if (DistanceToFrontBackPlane < 0)
		{
			MoveDirection *= -1.0f;
		}

		ACharacter* Character = Cast<ACharacter>(Pawn);
		
		if (Character)
		{
			if (USDCharacterMovementComponent* CharacterMovementComponent = Cast<USDCharacterMovementComponent>(Character->GetCharacterMovement()))
			{
				bIsAccelerating = CharacterMovementComponent->GetCurrentAcceleration().Size() > KINDA_SMALL_NUMBER;
				bIsSprinting = CharacterMovementComponent->IsSprinting();
			}
		}

		if (IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(Pawn))
		{
			FGameplayTagContainer GameplayTagContainer;
			GameplayTagAssetInterface->GetOwnedGameplayTags(GameplayTagContainer);
			bIsStunned = GameplayTagContainer.HasTagExact(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun")));
		}

		bFullBody = GetCurveValue(FName(TEXT("FullBody"))) > KINDA_SMALL_NUMBER;
	}
}
