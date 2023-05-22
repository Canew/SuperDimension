// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/SDANS_ApplyGameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

FString USDANS_ApplyGameplayEffect::GetNotifyName_Implementation() const
{
	return TEXT("ApplyGameplayEffect");
}

void USDANS_ApplyGameplayEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (GameplayEffectClasses.Num() == 0)
	{
		return;
	}

	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent())
		{
			for (TSubclassOf<class UGameplayEffect> GameplayEffectClass : GameplayEffectClasses)
			{
				FGameplayEffectSpecHandle GameplayEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, UGameplayEffect::INVALID_LEVEL, AbilitySystemComponent->MakeEffectContext());
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data);
				ActiveGameplayEffectHandles.Add(ActiveGameplayEffectHandle);
			}
		}
	}
}

void USDANS_ApplyGameplayEffect::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void USDANS_ApplyGameplayEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent())
		{
			for (FActiveGameplayEffectHandle ActiveGameplayEffectHandle : ActiveGameplayEffectHandles)
			{
				AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle);
			}
		}
	}
}
