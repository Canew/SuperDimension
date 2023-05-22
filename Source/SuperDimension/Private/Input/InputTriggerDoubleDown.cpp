// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputTriggerDoubleDown.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedPlayerInput.h"

ETriggerState UInputTriggerDoubleDown::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	if (IsActuated(ModifiedValue) && !IsActuated(LastValue))
	{
		const float CurrentTime = PlayerInput->GetOuterAPlayerController()->GetWorld()->GetRealTimeSeconds();
		if (CurrentTime - LastTappedTime < Delay)
		{
			LastTappedTime = 0.0f;
			return ETriggerState::Triggered;
		}

		LastTappedTime = CurrentTime;
	}

	return ETriggerState::None;
}