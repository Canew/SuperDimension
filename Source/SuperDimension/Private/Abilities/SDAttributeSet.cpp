// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

void USDAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetMaxHealth(), 0.0f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		SetAttackPower(FMath::Clamp(GetAttackPower(), 0.0f, GetAttackPower()));
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		HandleDamageAttributeExecute(Data);
	}
}

void USDAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USDAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USDAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USDAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
}

void USDAttributeSet::HandleDamageAttributeExecute(const FGameplayEffectModCallbackData& Data)
{
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();

	UAbilitySystemComponent* SourceAbilitySystemComponent = Context.GetOriginalInstigatorAbilitySystemComponent();
	TWeakObjectPtr<APlayerController> SourcePlayerController = Cast<APlayerController>(SourceAbilitySystemComponent->AbilityActorInfo->PlayerController);
	TWeakObjectPtr<AActor> SourceActor = SourceAbilitySystemComponent->AbilityActorInfo->AvatarActor;

	TWeakObjectPtr<APlayerController> TargetPlayerController = Cast<APlayerController>(Data.Target.AbilityActorInfo->PlayerController);
	TWeakObjectPtr<AActor> TargetActor = Data.Target.AbilityActorInfo->AvatarActor;

	// Store a local copy of the amount of damage done and clear the damage attribute
	float LocalDamage = GetDamage();
	SetDamage(0.0f);
	if (GetHealth() > 0.0f)
	{
		if (LocalDamage > 0.0f)
		{
			// Apply the health change and then clamp it
			const float NewHealth = GetHealth() - LocalDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}
}

void USDAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USDAttributeSet, Health, OldHealth);
}

void USDAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USDAttributeSet, MaxHealth, OldMaxHealth);
}

void USDAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USDAttributeSet, AttackPower, OldAttackPower);
}
