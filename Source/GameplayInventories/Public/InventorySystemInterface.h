// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventorySystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventorySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYINVENTORIES_API IInventorySystemInterface
{
	GENERATED_BODY()

public:
	virtual class UInventorySystemComponent* GetInventorySystemComponent() const = 0;
};
