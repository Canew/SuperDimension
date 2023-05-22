// Fill out your copyright notice in the Description page of Project Settings.


#include "SDPlayerState.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "Abilities/SDAttributeSet.h"

ASDPlayerState::ASDPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystem = CreateDefaultSubobject<USDAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<USDAttributeSet>(TEXT("AttributeSet"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	NetUpdateFrequency = 60.0f;
}

void ASDPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASDPlayerState::BeginPlay()
{
	Super::BeginPlay();


}

UAbilitySystemComponent* ASDPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

UAttributeSet* ASDPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

