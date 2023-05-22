


#include "Abilities/SDGA_Guard.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectBlockedImmunity.h"
#include "Abilities/SDGE_EnableCounter.h"
#include "Abilities/SDGE_RecoilGuard.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

USDGA_Guard::USDGA_Guard()
{
	GuardRecoilTime = 0.0f;
	CounterTimeLimit = 0.0f;
	bGuardSuccessfully = false;
	bEncounterResponseGameplayEffectTag = false;
	bAlreadySuspendedGameplayEffectApplied = false;
}

void USDGA_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	bGuardSuccessfully = false;
	bEncounterResponseGameplayEffectTag = false;
	bAlreadySuspendedGameplayEffectApplied = false;
	SuspendedGameplayEffectSpecHandles.Reset();

	UAbilityTask_PlayMontageAndWait* PlayGuardMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GuardMontage, 1.0f, NAME_None, false);
	if (PlayGuardMontageAndWait)
	{
		PlayGuardMontageAndWait->OnCompleted.AddDynamic(this, &USDGA_Guard::HandleMontageCompleted);
		PlayGuardMontageAndWait->OnBlendOut.AddDynamic(this, &USDGA_Guard::HandleMontageBlendOut);
		PlayGuardMontageAndWait->OnInterrupted.AddDynamic(this, &USDGA_Guard::HandleMontageInterrupted);
		PlayGuardMontageAndWait->OnCancelled.AddDynamic(this, &USDGA_Guard::HandleMontageCancelled);
		PlayGuardMontageAndWait->ReadyForActivation();

		PlayGuardActivationParticle();
	}

	if (UAbilityTask_WaitGameplayEffectBlockedImmunity* WaitGameplayEffectBlockedImmunity = UAbilityTask_WaitGameplayEffectBlockedImmunity::WaitGameplayEffectBlockedByImmunity(this, FGameplayTagRequirements(), FGameplayTagRequirements()))
	{
		WaitGameplayEffectBlockedImmunity->Blocked.AddDynamic(this, &USDGA_Guard::HandleGameplayEffectBlocked);
		WaitGameplayEffectBlockedImmunity->ReadyForActivation();
	}
}

void USDGA_Guard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	USkeletalMeshComponent* SkeletalMeshComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<USkeletalMeshComponent>();
	if (BodyPhysicalMaterial && SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetPhysMaterialOverride(BodyPhysicalMaterial);
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		for (FActiveGameplayEffectHandle ImmuneGameplayEffectHandle : ImmuneGameplayEffectHandles)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(ImmuneGameplayEffectHandle);
		}

		AbilitySystemComponent->RemoveActiveGameplayEffect(RecoilGuardEffectHandle);

		ApplySuspendedGameplayEffects();
	}
}

void USDGA_Guard::PlayGuardActivationParticle()
{
	if (!GuardActivationParticle)
	{
		return;
	}

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (GuardTriggerParticleComponent)
		{
			GuardTriggerParticleComponent->SetVisibility(false);
			GuardTriggerParticleComponent->DeactivateSystem();
			GuardTriggerParticleComponent->DestroyComponent();
			GuardTriggerParticleComponent = nullptr;
		}

		GuardActivationParticleComponent = UGameplayStatics::SpawnEmitterAttached(GuardActivationParticle, AvatarCharacter->GetMesh(), ParticleAttachName, LocationOffset, RotationOffset, EAttachLocation::SnapToTarget);
	}
}

void USDGA_Guard::PlayGuardTriggerParticle()
{
	if (!GuardTriggerParticle)
	{
		return;
	}

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (GuardActivationParticleComponent)
		{
			GuardActivationParticleComponent->SetVisibility(false);
			GuardActivationParticleComponent->DeactivateSystem();
			GuardActivationParticleComponent->DestroyComponent();
			GuardActivationParticleComponent = nullptr;
		}

		GuardTriggerParticleComponent = UGameplayStatics::SpawnEmitterAttached(GuardTriggerParticle, AvatarCharacter->GetMesh(), ParticleAttachName, LocationOffset, RotationOffset, EAttachLocation::SnapToTarget);
	}

	UGameplayStatics::PlaySoundAtLocation(GetAvatarActorFromActorInfo(), GuardTriggerSound, GetAvatarActorFromActorInfo()->GetActorLocation());
}

void USDGA_Guard::HandleMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Guard::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Guard::HandleMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Guard::HandleMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void USDGA_Guard::HandleConfirmAbilityEventReceived(FGameplayEventData Payload)
{
	Super::HandleConfirmAbilityEventReceived(Payload);

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		for (TSubclassOf<class UGameplayEffect> ImmuneGameplayEffectClass : ImmuneGameplayEffectClasses)
		{
			FGameplayEffectSpecHandle GameplayEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ImmuneGameplayEffectClass, UGameplayEffect::INVALID_LEVEL, AbilitySystemComponent->MakeEffectContext());
			FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data);
			ImmuneGameplayEffectHandles.Add(ActiveGameplayEffectHandle);
		}
	}

	USkeletalMeshComponent* SkeletalMeshComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<USkeletalMeshComponent>();
	if (BodyPhysicalMaterial && SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetPhysMaterialOverride(nullptr);
	}
}

void USDGA_Guard::HandleGameplayEffectBlocked(FGameplayEffectSpecHandle BlockedSpec, FActiveGameplayEffectHandle ImmunityGameplayEffectHandle)
{
	UAbilitySystemComponent* InstigatorAbilitySystemComponent = BlockedSpec.Data->GetEffectContext().GetOriginalInstigatorAbilitySystemComponent();
	TPair<UAbilitySystemComponent*, FGameplayEffectSpecHandle> SuspendedGameplayEffectSpecHandle;
	SuspendedGameplayEffectSpecHandle.Key = InstigatorAbilitySystemComponent;
	SuspendedGameplayEffectSpecHandle.Value = BlockedSpec;
	SuspendedGameplayEffectSpecHandles.Add(SuspendedGameplayEffectSpecHandle);

	FGameplayTagContainer BlockedSpecAssetTags;
	BlockedSpec.Data->GetAllAssetTags(BlockedSpecAssetTags);
	if (!BlockedSpecAssetTags.HasTag(ResponseGameplayEffectTag))
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Orange, FString::Printf(TEXT("Blocked")));

	bEncounterResponseGameplayEffectTag = true;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AActor* SourceActor = Cast<AActor>(BlockedSpec.Data->GetEffectContext().GetSourceObject());
	if (!IsInResponseAngle(AvatarActor, SourceActor))
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Orange, FString::Printf(TEXT("Blocked !IsInResponseAngle()")));
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	bGuardSuccessfully = true;
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystemComponent && !AbilitySystemComponent->IsPendingKill())
	{
		PlayGuardTriggerParticle();

		FGameplayEventData GameplayEventData;
		AbilitySystemComponent->HandleGameplayEvent(TriggerEventTag, &GameplayEventData);

		if (CounterTimeLimit > 0.0f)
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
			FGameplayEffectSpecHandle EnableCounterEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(USDGE_EnableCounter::StaticClass(), UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
			EnableCounterEffectSpecHandle.Data->SetDuration(CounterTimeLimit, true);
			CounterEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EnableCounterEffectSpecHandle.Data);
		}

		// apply delay if block successfully
		if (GuardRecoilTime > 0.0f)
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
			FGameplayEffectSpecHandle RecoilGuardEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(USDGE_RecoilGuard::StaticClass(), UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
			RecoilGuardEffectSpecHandle.Data->SetDuration(GuardRecoilTime, true);
			RecoilGuardEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*RecoilGuardEffectSpecHandle.Data);
			AvatarActor->GetWorldTimerManager().SetTimer(GuardRecoilTimerHandle, FTimerDelegate::CreateLambda(
				[this]() -> void
				{
					EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
				}), GuardRecoilTime, false);
		}
		else
		{
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		}
	}
}

bool USDGA_Guard::IsInResponseAngle(AActor* Center, AActor* Other)
{
	if (!Center || !Other)
	{
		return false;
	}

	FVector CenterForwardVector = Center->GetActorForwardVector().GetSafeNormal2D();
	FVector CenterToOtherVector = (Other->GetActorLocation() - Center->GetActorLocation()).GetSafeNormal2D();
	float DegreeValue = FMath::RadiansToDegrees(FMath::Acos(CenterForwardVector | CenterToOtherVector));

	if (ResponseAngle > DegreeValue)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void USDGA_Guard::ApplySuspendedGameplayEffects()
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (!bAlreadySuspendedGameplayEffectApplied && bEncounterResponseGameplayEffectTag && !bGuardSuccessfully)
		{
			bAlreadySuspendedGameplayEffectApplied = true;
			for (TPair<class UAbilitySystemComponent*, FGameplayEffectSpecHandle> SuspendedGameplayEffectSpecHandle : SuspendedGameplayEffectSpecHandles)
			{
				UAbilitySystemComponent* SourceAbilitySystemComponent = SuspendedGameplayEffectSpecHandle.Key;
				FGameplayEffectSpecHandle GameplayEffectSpecHandle = SuspendedGameplayEffectSpecHandle.Value;
				if (SourceAbilitySystemComponent && GameplayEffectSpecHandle.IsValid())
				{
					SourceAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data, AbilitySystemComponent);
				}
			}
		}
	}
}

void USDGA_Guard::SetPhysicalMaterial()
{
	if (USkeletalMeshComponent* SkeletalMeshComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<USkeletalMeshComponent>())
	{
		SkeletalMeshComponent->SetPhysMaterialOverride(nullptr);
	}
}
