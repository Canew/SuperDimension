// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGA_Evade.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

bool USDGA_Evade::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void USDGA_Evade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (EvadeMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayEvadeMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, EvadeMontage);
		if (PlayEvadeMontageAndWait)
		{
			PlayEvadeMontageAndWait->OnCompleted.AddDynamic(this, &USDGA_Evade::HandleMontageCompleted);
			PlayEvadeMontageAndWait->OnBlendOut.AddDynamic(this, &USDGA_Evade::HandleMontageBlendOut);
			PlayEvadeMontageAndWait->OnInterrupted.AddDynamic(this, &USDGA_Evade::HandleMontageInterrupted);
			PlayEvadeMontageAndWait->OnCancelled.AddDynamic(this, &USDGA_Evade::HandleMontageCancelled);
			PlayEvadeMontageAndWait->ReadyForActivation();

			GetAvatarActorFromActorInfo()->GetWorldTimerManager().SetTimer(AppearanceTimerHandle, this, &USDGA_Evade::TickAppearanceOpacity, AppearanceTickInterval, true, EvadeMontage->BlendIn.GetBlendTime());

			return;
		}
	}
}

void USDGA_Evade::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	GetAvatarActorFromActorInfo()->GetWorldTimerManager().ClearTimer(AppearanceTimerHandle);
	ClearAppearanceOpacity();
}

void USDGA_Evade::HandleMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Evade::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Evade::HandleMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Evade::HandleMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void USDGA_Evade::TickAppearanceOpacity()
{
	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* AvatarMesh = AvatarCharacter->GetMesh())
		{
			float AppearanceOpacity;
			if (AvatarMesh->GetAnimInstance()->GetCurveValue(TEXT("AppearanceOpacity"), AppearanceOpacity))
			{
				int32 MaterialNum = AvatarMesh->GetMaterials().Num();
				for (int MaterialIndex = 0; MaterialIndex < MaterialNum; MaterialIndex++)
				{
					UMaterialInstanceDynamic* MaterialInstanceDynamic = AvatarMesh->CreateDynamicMaterialInstance(MaterialIndex, AvatarMesh->GetMaterials()[MaterialIndex]);
					MaterialInstanceDynamic->SetScalarParameterValue(TEXT("AppearanceOpacity"), AppearanceOpacity);
				}
			}
		}
	}
}

void USDGA_Evade::ClearAppearanceOpacity()
{
	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* AvatarMesh = AvatarCharacter->GetMesh())
		{
			int32 MaterialNum = AvatarMesh->GetMaterials().Num();
			for (int MaterialIndex = 0; MaterialIndex < MaterialNum; MaterialIndex++)
			{
				UMaterialInstanceDynamic* MaterialInstanceDynamic = AvatarMesh->CreateDynamicMaterialInstance(MaterialIndex, AvatarMesh->GetMaterials()[MaterialIndex]);
				MaterialInstanceDynamic->SetScalarParameterValue(TEXT("AppearanceOpacity"), 1.01f);
			}
		}
	}
}