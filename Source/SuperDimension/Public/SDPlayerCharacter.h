// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDCharacter.h"
#include "InputAction.h"
#include "SDPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API ASDPlayerCharacter : public ASDCharacter
{
	GENERATED_BODY()

public:
	ASDPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
#pragma region Input
public:
	virtual class UInputMappingContext* GetInputMappingContext() const;
	virtual class UInputAction* GetInteractInputAction() const;
	virtual void BindAbilityInputAction(TSubclassOf<class USDGameplayAbility> AbilityClass);

protected:
	virtual void HandleMoveInputAction(const FInputActionInstance& ActionInstance);
	virtual void HandleLookInputAction(const FInputActionInstance& ActionInstance);
	virtual void HandleInteractInputAction(const FInputActionInstance& ActionInstance);
	virtual void HandleAbilityInputActionStarted(TSubclassOf<class USDGameplayAbility> AbilityClass);
	virtual void HandleAbilityInputActionTriggered(TSubclassOf<class USDGameplayAbility> AbilityClass);
	virtual void HandleAbilityInputActionReleased(TSubclassOf<class USDGameplayAbility> AbilityClass);

	virtual void MoveForward(float AxisValue);
	virtual void MoveRight(float AxisValue);
	virtual void Turn(float AxisValue);
	virtual void LookUp(float AxisValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* InteractInputAction;

#pragma endregion


#pragma region Camera
protected:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

#pragma endregion


#pragma region Interaction
protected:
	UPROPERTY(VisibleAnywhere)
	class USDInteractionComponent* InteractionComponent;



#pragma endregion
};
