// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API ASDPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ASDPlayerState();

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public:
	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UAttributeSet* GetAttributeSet() const;

	template <typename T>
	T* GetAttributeSet() const
	{
		return Cast<T>(GetAttributeSet());
	}

protected:
	UPROPERTY(VisibleAnywhere)
	class USDAbilitySystemComponent* AbilitySystem;

	UPROPERTY()
	class USDAttributeSet* AttributeSet;
};
