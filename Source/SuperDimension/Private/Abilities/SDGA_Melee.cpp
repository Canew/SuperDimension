// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGA_Melee.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/SDGT_WaitOverlapEvent.h"
#include "GameFramework/Character.h"
#include "Weapons/SDWeapon.h"
#include "AI/SDAIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "SDCharacter.h"

USDGA_Melee::USDGA_Melee()
{
	BeginOverlapTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.BeginOverlap"));
	EndOverlapTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.EndOverlap"));

	PushBackForce = 0.0f;
	PushBackStunDuration = 0.0f;
}

void USDGA_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;

	AffectedActors.Reset();

	UAbilityTask_PlayMontageAndWait* PlayMeleeMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MeleeMontage, 1.0f, NAME_None, false);
	if (PlayMeleeMontageAndWait)
	{
		PlayMeleeMontageAndWait->OnCompleted.AddDynamic(this, &USDGA_Melee::HandleMontageCompleted);
		PlayMeleeMontageAndWait->OnBlendOut.AddDynamic(this, &USDGA_Melee::HandleMontageBlendOut);
		PlayMeleeMontageAndWait->OnInterrupted.AddDynamic(this, &USDGA_Melee::HandleMontageInterrupted);
		PlayMeleeMontageAndWait->OnCancelled.AddDynamic(this, &USDGA_Melee::HandleMontageCancelled);
		PlayMeleeMontageAndWait->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitBeginOverlapEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, BeginOverlapTag);
		if (WaitBeginOverlapEvent)
		{
			WaitBeginOverlapEvent->EventReceived.AddDynamic(this, &USDGA_Melee::HandleBeginOverlapEventReceived);
			WaitBeginOverlapEvent->ReadyForActivation();

			UAbilityTask_WaitGameplayEvent* WaitEndOverlapEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndOverlapTag);
			if (WaitEndOverlapEvent)
			{
				WaitEndOverlapEvent->EventReceived.AddDynamic(this, &USDGA_Melee::HandleEndOverlapEventReceived);
				WaitEndOverlapEvent->ReadyForActivation();
			}
		}
	}
}

void USDGA_Melee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}

ASDWeapon* USDGA_Melee::GetAvatarWeapon(FName AttachSocketName)
{
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		TArray<AActor*> ChildActors;
		AvatarActor->GetAttachedActors(ChildActors);
		for (AActor* ChildActor : ChildActors)
		{
			ASDWeapon* EquippedWeaponActor = Cast<ASDWeapon>(ChildActor);
			if (EquippedWeaponActor && EquippedWeaponActor->GetAttachParentSocketName() == AttachSocketName)
			{
				return EquippedWeaponActor;
			}
		}
	}

	return nullptr;
}

void USDGA_Melee::HandleConfirmAbilityEventReceived(FGameplayEventData Payload)
{
	Super::HandleConfirmAbilityEventReceived(Payload);

	// Report 
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		if (UWorld* World = AvatarActor->GetWorld())
		{
			TArray<FHitResult> HitResults;
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(AvatarActor);
			World->SweepMultiByProfile(HitResults, AvatarActor->GetActorLocation(), AvatarActor->GetActorLocation() + ExpectedEffectiveRange * AvatarActor->GetActorForwardVector(), FQuat::Identity, MeleeCollisionProfileName, FCollisionShape::MakeSphere(25.0f), CollisionQueryParams);
			for (const FHitResult& HitResult : HitResults)
			{
				APawn* HitResultPawn = Cast<ACharacter>(HitResult.GetActor());
				if (HitResultPawn && !HitResultPawn->IsPlayerControlled())
				{
					if (ASDAIController* HitResultAIController = Cast<ASDAIController>(HitResultPawn->GetController()))
					{
						HitResultAIController->PredictBeingHit();
					}
				}
			}
		}
	}
}

void USDGA_Melee::HandleMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Melee::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Melee::HandleMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Melee::HandleMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void USDGA_Melee::HandleBeginOverlapEventReceived(FGameplayEventData Payload)
{
	BeginWaitOverlapEvent();
}

void USDGA_Melee::BeginWaitOverlapEvent()
{
	FCollisionShape MeleeCollisionShape;
	switch (MeleeCollisionShapeType)
	{
	case EMeleeCollisionShape::Box:
		MeleeCollisionShape.SetBox(BoxHalfExtent);
		break;

	case EMeleeCollisionShape::Sphere:
		MeleeCollisionShape.SetSphere(SphereRadius);
		break;

	case EMeleeCollisionShape::Capsule:
		MeleeCollisionShape.SetCapsule(CapsuleRadius, CapsuleHalfHeight);
		break;
	}

	AvatarWeapon = GetAvatarWeapon(MeleeAttachSocketName);

	FWaitOverlapEventParams WaitOverlapEventParams;
	WaitOverlapEventParams.CollisionAttachSocketName = MeleeAttachSocketName;
	WaitOverlapEventParams.CollisionProfileName = MeleeCollisionProfileName;
	WaitOverlapEventParams.LocationOffset = (AvatarWeapon.IsValid()) ? AvatarWeapon->GetWeaponCollisionCenterLocationOffset() : FVector::ZeroVector;
	WaitMeleeOverlapEvent = USDGT_WaitOverlapEvent::CreateWaitOverlapEvent(this, NAME_None, MeleeCollisionShape, WaitOverlapEventParams);
	if (WaitMeleeOverlapEvent)
	{
		WaitMeleeOverlapEvent->OnOverlap.AddDynamic(this, &USDGA_Melee::HandleMeleeBeginOverlap);
		WaitMeleeOverlapEvent->TickInterval = 0.05f;
		WaitMeleeOverlapEvent->ReadyForActivation();
	}
}

void USDGA_Melee::HandleEndOverlapEventReceived(FGameplayEventData Payload)
{
	if (WaitMeleeOverlapEvent)
	{
		WaitMeleeOverlapEvent->OnOverlap.RemoveDynamic(this, &USDGA_Melee::HandleMeleeBeginOverlap);
		WaitMeleeOverlapEvent->EndTask();
	}
}

void USDGA_Melee::HandleMeleeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || AvatarActor == OtherActor)
	{
		return;
	}

	if (AffectedActors.Find(OtherActor) != INDEX_NONE)
	{
		return;
	}
	AffectedActors.AddUnique(OtherActor);

	bool bHostile = false;
	IGenericTeamAgentInterface* OwnerGenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerGenericTeamAgentInterface)
	{
		ETeamAttitude::Type TeamAttitude = OwnerGenericTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor);
		switch (TeamAttitude)
		{
		case ETeamAttitude::Friendly:
			return;
			break;

		case ETeamAttitude::Hostile:
		{
			bHostile = true;
			break;
		}

		case ETeamAttitude::Neutral:
			break;
		}
	}

	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* OtherAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (OwnerAbilitySystemComponent)
	{
		// Apply gameplay effect
		if (OtherAbilitySystemComponent && MeleeGameplayEffectClass)
		{
			FGameplayEffectSpecHandle MeleeGameplayEffectSpecHandle = MakeOutgoingGameplayEffectSpec(MeleeGameplayEffectClass);
			OwnerAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*MeleeGameplayEffectSpecHandle.Data.Get(), OtherAbilitySystemComponent);
		}

		// Apply pushback
		if (ASDCharacter* OtherCharacter = Cast<ASDCharacter>(OtherActor))
		{
			if (PushBackForce > 0.0f)
			{
				FVector PushBackDirection = (OtherActor->GetActorLocation() - AvatarActor->GetActorLocation()).GetSafeNormal2D();
				OtherCharacter->Pushback(GetAvatarActorFromActorInfo(), PushBackForce * PushBackDirection, PushBackStunDuration);
			}
		}

		// Play gameplay cue
		if (MeleeGameplayCueTag.IsValid())
		{
			FGameplayEffectContextHandle GameplayEffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
			GameplayEffectContextHandle.AddSourceObject(AvatarActor);
			GameplayEffectContextHandle.AddHitResult(SweepResult, true);

			FGameplayCueParameters GameplayCueParameters;
			GameplayCueParameters.EffectCauser = GetAvatarActorFromActorInfo();
			GameplayCueParameters.EffectContext = GameplayEffectContextHandle;
			GameplayCueParameters.PhysicalMaterial = SweepResult.PhysMaterial.Get();
			OtherAbilitySystemComponent
				? OtherAbilitySystemComponent->ExecuteGameplayCue(MeleeGameplayCueTag, GameplayCueParameters)
				: OwnerAbilitySystemComponent->ExecuteGameplayCue(MeleeGameplayCueTag, GameplayCueParameters);
		}
	}
}
