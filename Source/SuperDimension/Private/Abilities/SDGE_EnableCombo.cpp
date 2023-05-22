// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGE_EnableCombo.h"

USDGE_EnableCombo::USDGE_EnableCombo()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Character.State.EnableCombo")));
}