// Fill out your copyright notice in the Description page of Project Settings.


#include "SDPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UMG/SDHUDWidget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InventorySystemComponent.h"

ASDPlayerController::ASDPlayerController()
{
	InventorySystemComponent = CreateDefaultSubobject<UInventorySystemComponent>(TEXT("InventorySystemComponent"));

	ActionBufferDuration = 0.5f;
}

void ASDPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = -65.0f;
		PlayerCameraManager->ViewPitchMax = 65.0f;
	}


}

void ASDPlayerController::BeginPlay()
{
	Super::BeginPlay(); 

	SetGameInputMode(true);

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<USDHUDWidget>(this, HUDWidgetClass, TEXT("HUD"));
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

void ASDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (CursorInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(CursorInputAction, ETriggerEvent::Triggered, this, &ASDPlayerController::HandleCursorInputAction);
		}
		if (InventoryInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(InventoryInputAction, ETriggerEvent::Triggered, this, &ASDPlayerController::HandleInventoryInputAction);
		}
	}

	// Get the Enhanced Input Local Player Subsystem from the Local Player related to Player Controller.
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

UInputMappingContext* ASDPlayerController::GetInputMappingContext() const
{
	return InputMappingContext;
}

void ASDPlayerController::SetGameInputMode(bool bGameInputMode)
{
	if (bGameInputMode)
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
	}
	else
	{
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(true);
	}
}

void ASDPlayerController::HandleCursorInputAction(const FInputActionInstance& ActionInstance)
{
	if (ShouldShowMouseCursor())
	{
		SetGameInputMode(true);
	}
	else
	{
		SetGameInputMode(false);
	}
}

void ASDPlayerController::HandleInventoryInputAction(const FInputActionInstance& ActionInstance)
{
	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
		{
			InventoryWidget->RemoveFromViewport();
			SetGameInputMode(true);
		}
		else
		{
			InventoryWidget->AddToViewport();
			SetGameInputMode(false);
		}
	}
	else
	{
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass, TEXT("Inventory"));
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();
				SetGameInputMode(false);
			}
		}
	}
}

void ASDPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (TeamId != NewTeamId)
	{
		TeamId = NewTeamId;
	}
}

FGenericTeamId ASDPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void ASDPlayerController::SetRecentInputAction(UInputAction* NewRecentInputAction)
{
	if (CanReceiveRecentInputAction())
	{
		RecentInputAction = NewRecentInputAction;
		GetWorld()->GetTimerManager().SetTimer(ActionBufferTimerHandle, this, &ASDPlayerController::ClearRecentInputAction, ActionBufferDuration, false);
	}
}

void ASDPlayerController::InjectRecentInputForAction()
{
	if (!RecentInputAction)
	{
		return;
	}

	if (UEnhancedPlayerInput* EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(PlayerInput))
	{
		FInputActionValue InputActionValue = true;
		EnhancedPlayerInput->InjectInputForAction(RecentInputAction, InputActionValue);

		GetWorld()->GetTimerManager().ClearTimer(ActionBufferTimerHandle);
		ClearRecentInputAction();
	}
}

void ASDPlayerController::ClearRecentInputAction()
{
	RecentInputAction = nullptr;
}

bool ASDPlayerController::CanReceiveRecentInputAction() const
{
	return bCanReceiveRecentInputAction;
}

void ASDPlayerController::SetCanReceiveRecentInputAction(bool CanReceiveRecentInputAction)
{
	bCanReceiveRecentInputAction = CanReceiveRecentInputAction;
}

void ASDPlayerController::SetHUDBossActor(AActor* Actor)
{
	if (HUDWidget)
	{
		HUDWidget->SetBossActor(Actor);
	}
}

UInventorySystemComponent* ASDPlayerController::GetInventorySystemComponent() const
{
	return InventorySystemComponent;
}
