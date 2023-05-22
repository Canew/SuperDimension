


#include "Abilities/SDGA_Climb.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"

USDGA_Climb::USDGA_Climb()
{
	MaxClimbHeight = 120.0f;
	TraceDistance = 60.0f;
	TraceHeight = 250.0f;
}

bool USDGA_Climb::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!AvatarCharacter || !ClimbMontage)
	{
		return false;
	}

	FHitResult ForwardHitResult;
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	bool bHitForwardTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		ForwardHitResult,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorUpVector() * AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 1.5f,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * TraceDistance + AvatarCharacter->GetActorUpVector() * AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 1.5f,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()));

	FHitResult VerticalHitResult;
	bool bHitVerticalTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		VerticalHitResult,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorUpVector() * TraceHeight + AvatarCharacter->GetActorForwardVector() * TraceDistance,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * TraceDistance + AvatarCharacter->GetActorUpVector() * AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 1.5f,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()));

	float HeightDifference = AvatarCharacter->GetCapsuleComponent()->GetComponentLocation().Z - VerticalHitResult.Location.Z;
	return bHitForwardTrace && bHitVerticalTrace  && (HeightDifference > -MaxClimbHeight && HeightDifference < 0.0f);
}

void USDGA_Climb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;

	ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!AvatarCharacter)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Forward trace
	FHitResult ForwardHitResult;
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	bool bHitForwardTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		ForwardHitResult,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorUpVector() * AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 1.5f,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * TraceDistance + AvatarCharacter->GetActorUpVector() * AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 1.5f,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()));

	// Vertical trace
	FHitResult VerticalHitResult;
	bool bHitVerticalTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		VerticalHitResult,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorUpVector() * TraceHeight + AvatarCharacter->GetActorForwardVector() * TraceDistance,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * TraceDistance + AvatarCharacter->GetActorUpVector() * AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 1.5f,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()));

	// Play climb montage
	if (UAbilityTask_PlayMontageAndWait* PlayClimbMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ClimbMontage))
	{
		OriginalHalfHeight = AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		ClimbingHalfHeight = OriginalHalfHeight * 0.6f;
		AvatarCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(ClimbingHalfHeight, true);
		AvatarCharacter->GetCharacterMovement()->StopMovementImmediately();
		AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		FVector NewLocation(
			ForwardHitResult.Location.X + (ForwardHitResult.Normal.X * 10.0f),
			ForwardHitResult.Location.Y + (ForwardHitResult.Normal.Y * 10.0f),
			VerticalHitResult.Location.Z - 115.0f);
		AvatarCharacter->SetActorLocation(NewLocation);

		FRotator NewRotation = AvatarCharacter->GetActorRotation();
		NewRotation.Yaw = ForwardHitResult.Normal.Rotation().Yaw + 180.0f;
		AvatarCharacter->SetActorRotation(NewRotation);

		PlayClimbMontageAndWait->OnCompleted.AddDynamic(this, &USDGA_Climb::HandleMontageCompleted);
		PlayClimbMontageAndWait->OnBlendOut.AddDynamic(this, &USDGA_Climb::HandleMontageBlendOut);
		PlayClimbMontageAndWait->OnInterrupted.AddDynamic(this, &USDGA_Climb::HandleMontageInterrupted);
		PlayClimbMontageAndWait->OnCancelled.AddDynamic(this, &USDGA_Climb::HandleMontageCancelled);
		PlayClimbMontageAndWait->ReadyForActivation();
	}

}

void USDGA_Climb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor))
	{
		AvatarCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalHalfHeight, true);
		AvatarCharacter->GetCharacterMovement()->StopMovementImmediately();
		AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void USDGA_Climb::HandleMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Climb::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Climb::HandleMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Climb::HandleMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
