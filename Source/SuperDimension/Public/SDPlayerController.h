// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "InventorySystemInterface.h"
#include "InputAction.h"
#include "SDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API ASDPlayerController : public APlayerController, public IGenericTeamAgentInterface, public IInventorySystemInterface
{
	GENERATED_BODY()

public:
	ASDPlayerController();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

#pragma region Input
protected:
	virtual class UInputMappingContext* GetInputMappingContext() const;
	virtual void SetGameInputMode(bool bGameInputMode);

	virtual void HandleCursorInputAction(const FInputActionInstance& ActionInstance);
	virtual void HandleInventoryInputAction(const FInputActionInstance& ActionInstance);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* CursorInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* InventoryInputAction;

#pragma endregion


#pragma region GenericTeamAgentInterface
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	FGenericTeamId TeamId;

#pragma endregion


#pragma region Action Buffer
public:
	virtual void SetRecentInputAction(class UInputAction* NewRecentInputAction);
	virtual void InjectRecentInputForAction();
	virtual void ClearRecentInputAction();

	virtual bool CanReceiveRecentInputAction() const;
	virtual void SetCanReceiveRecentInputAction(bool CanReceiveInput);
	
protected:
	class UInputAction* RecentInputAction;

	FTimerHandle ActionBufferTimerHandle;
	float ActionBufferDuration;
	bool bCanReceiveRecentInputAction;

#pragma endregion


#pragma region HUD
public:
	virtual void SetHUDBossActor(AActor* Actor);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD | Widget")
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	UPROPERTY()
	class USDHUDWidget* HUDWidget;

#pragma endregion


#pragma region GameplayInventories
public:
	UInventorySystemComponent* GetInventorySystemComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UInventorySystemComponent* InventorySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Inventories | Widget")
	TSubclassOf<class UUserWidget> InventoryWidgetClass;

	UPROPERTY()
	class UUserWidget* InventoryWidget;

#pragma endregion
};
