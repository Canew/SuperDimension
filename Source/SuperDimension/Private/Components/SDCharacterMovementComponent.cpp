// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SDCharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayTagAssetInterface.h"
#include "AI/SDAIController.h"

USDCharacterMovementComponent::USDCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
	PassiveAISpeedMultiplier = 0.35f;
}

void USDCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsInWater)
	{
		if (!IsClimbing())
		{
			float OwnerLocationZ = GetOwner()->GetActorLocation().Z;
			if (OwnerLocationZ < WaterLevelLocationZ)
			{
				if (IsSwimming())
				{
					FVector NewOwnerLocation = GetOwner()->GetActorLocation();
					NewOwnerLocation.Z = FMath::FInterpTo(GetOwner()->GetActorLocation().Z, WaterLevelLocationZ, DeltaTime, Buoyancy);
					GetOwner()->SetActorLocation(NewOwnerLocation);
				}
				else
				{
					StartSwimming();
				}
			}
			else if (IsSwimming())
			{
				EndSwimming();
			}
		}
	}
	else
	{
		EndSwimming();
	}
}

float USDCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();
	
	if (IsImmovable() || IsStunned() || IsClimbing())
	{
		return 0.0f;
	}

	if (IsSprinting())
	{
		MaxSpeed *= SprintSpeedMultiplier;
	}
	
	if (IsPassiveAI())
	{
		MaxSpeed *= PassiveAISpeedMultiplier;
	}

	return MaxSpeed;
}

float USDCharacterMovementComponent::GetMaxAcceleration() const
{
	float MaxAccelerationSpeed = Super::GetMaxAcceleration();

	if (IsImmovable() || IsClimbing())
	{
		return 0.0f;
	}

	return MaxAccelerationSpeed;
}

bool USDCharacterMovementComponent::IsSprinting() const
{
	IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(GetOwner());
	if (GameplayTagAssetInterface)
	{
		FGameplayTagContainer SprintTagContainer;
		GameplayTagAssetInterface->GetOwnedGameplayTags(SprintTagContainer);

		FGameplayTag SprintTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Sprint")));
		return SprintTagContainer.HasTag(SprintTag);
	}

	return false;
}

bool USDCharacterMovementComponent::IsImmovable() const
{
	IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(GetOwner());
	if (GameplayTagAssetInterface)
	{
		FGameplayTagContainer ImmovableTagContainer;
		GameplayTagAssetInterface->GetOwnedGameplayTags(ImmovableTagContainer);

		FGameplayTag ImmovableTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Immovable")));
		return ImmovableTagContainer.HasTag(ImmovableTag);
	}

	return false;
}

bool USDCharacterMovementComponent::IsStunned() const
{
	IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(GetOwner());
	if (GameplayTagAssetInterface)
	{
		FGameplayTagContainer StunTagContainer;
		GameplayTagAssetInterface->GetOwnedGameplayTags(StunTagContainer);

		FGameplayTag StunTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun")));
		return StunTagContainer.HasTag(StunTag);
	}

	return false;
}

bool USDCharacterMovementComponent::IsPassiveAI() const
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (ASDAIController* AIController = OwnerPawn->GetController<ASDAIController>())
		{
			return !AIController->IsAggressive();
		}
	}

	return false;
}

void USDCharacterMovementComponent::EnterWater(float InWaterLevelLocationZ)
{
	bIsInWater = true;
	WaterLevelLocationZ = InWaterLevelLocationZ;
}

void USDCharacterMovementComponent::ExitWater()
{
	bIsInWater = false;
	EndSwimming();
}

void USDCharacterMovementComponent::StartSwimming()
{
	SetMovementMode(EMovementMode::MOVE_Swimming);
	GetPhysicsVolume()->bWaterVolume = true;
}

void USDCharacterMovementComponent::EndSwimming()
{
	if (IsSwimming() && !IsClimbing())
	{
		SetMovementMode(EMovementMode::MOVE_Walking);
		GetPhysicsVolume()->bWaterVolume = false;
	}
}

bool USDCharacterMovementComponent::IsClimbing() const
{
	IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(GetOwner());
	if (GameplayTagAssetInterface)
	{
		FGameplayTagContainer ClimbingTagContainer;
		GameplayTagAssetInterface->GetOwnedGameplayTags(ClimbingTagContainer);
		FGameplayTag ClimbingTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.Climb")));
		return ClimbingTagContainer.HasTag(ClimbingTag);
	}

	return false;
}
