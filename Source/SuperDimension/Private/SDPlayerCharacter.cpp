// Fill out your copyright notice in the Description page of Project Settings.


#include "SDPlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "Abilities/SDGameplayAbility.h"
#include "Components/SDInteractionComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "SDPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"

ASDPlayerCharacter::ASDPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 225.0f;
	SpringArm->SocketOffset = FVector(0.0f, 30.0f, 45.0f);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 25.0f;
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 45.0f));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->MaxAcceleration = 1280.0f;
	GetCharacterMovement()->MaxWalkSpeed = 320.0f;
	GetCharacterMovement()->JumpZVelocity = 380.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	InteractionComponent = CreateDefaultSubobject<USDInteractionComponent>(TEXT("Interaction Component"));
}

void ASDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ASDPlayerCharacter::HandleMoveInputAction);
		}
		if (LookInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ASDPlayerCharacter::HandleLookInputAction);
		}
		if (InteractInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Triggered, this, &ASDPlayerCharacter::HandleInteractInputAction);
		}

		for (const TSubclassOf<USDGameplayAbility>& DefaultAbility : DefaultAbilities)
		{
			BindAbilityInputAction(DefaultAbility);
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

UInputMappingContext* ASDPlayerCharacter::GetInputMappingContext() const
{
	return InputMappingContext;
}

UInputAction* ASDPlayerCharacter::GetInteractInputAction() const
{
	return InteractInputAction;
}

void ASDPlayerCharacter::BindAbilityInputAction(TSubclassOf<class USDGameplayAbility> AbilityClass)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!AbilityClass || !EnhancedInputComponent)
	{
		return;
	}

	if (USDGameplayAbility* AbilityDefaultObject = AbilityClass.GetDefaultObject())
	{
		UInputAction* AbilityInputAction = AbilityDefaultObject->AbilityInputAction;
		if (AbilityInputAction)
		{
			EnhancedInputComponent->BindAction(AbilityInputAction, ETriggerEvent::Started, this, &ASDPlayerCharacter::HandleAbilityInputActionStarted, AbilityClass);
			EnhancedInputComponent->BindAction(AbilityInputAction, ETriggerEvent::Triggered, this, &ASDPlayerCharacter::HandleAbilityInputActionTriggered, AbilityClass);
			EnhancedInputComponent->BindAction(AbilityInputAction, ETriggerEvent::Completed, this, &ASDPlayerCharacter::HandleAbilityInputActionReleased, AbilityClass);
		}
	}
}

void ASDPlayerCharacter::HandleMoveInputAction(const FInputActionInstance& ActionInstance)
{
	if (ActionInstance.GetValue().GetValueType() == EInputActionValueType::Axis2D)
	{
		FVector2D ActionVector2D = ActionInstance.GetValue().Get<FVector2D>();
		MoveForward(ActionVector2D.Y);
		MoveRight(ActionVector2D.X);
	}
}

void ASDPlayerCharacter::HandleLookInputAction(const FInputActionInstance& ActionInstance)
{
	if (ActionInstance.GetValue().GetValueType() == EInputActionValueType::Axis2D)
	{
		FVector2D ActionVector2D = ActionInstance.GetValue().Get<FVector2D>();
		Turn(ActionVector2D.X);
		LookUp(ActionVector2D.Y);
	}
}

void ASDPlayerCharacter::HandleInteractInputAction(const FInputActionInstance& ActionInstance)
{
	if (ActionInstance.GetValue().GetValueType() == EInputActionValueType::Boolean)
	{
		bool ActionValue = ActionInstance.GetValue().Get<bool>();
		if (ActionValue)
		{
			if (!InteractionComponent)
			{
				return;
			}

			TWeakObjectPtr<AActor> InteractionActor = InteractionComponent->GetCurrentInteractionActor();
			if (InteractionActor.IsValid())
			{
				FVector ToInteractionActorVector = (InteractionActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
				SetActorRotation(ToInteractionActorVector.Rotation());
				InteractionComponent->Interact();
				if (UAnimMontage* InteractMontage = InteractionComponent->GetInteractMontage())
				{
					PlayAnimMontage(InteractMontage);
				}
			}
		}
	}
}

void ASDPlayerCharacter::HandleAbilityInputActionStarted(TSubclassOf<class USDGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return;
	}

	if (USDAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent<USDAbilitySystemComponent>())
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass, true);
		if (AbilitySpec)
		{
			if (AbilitySpec->IsActive())
			{
				AbilitySystemComponent->AbilitySpecInputPressed(*AbilitySpec);
				AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec->Handle, AbilitySpec->ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void ASDPlayerCharacter::HandleAbilityInputActionTriggered(TSubclassOf<class USDGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return;
	}

	if (USDAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent<USDAbilitySystemComponent>())
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass, true);
		USDGameplayAbility* Ability = Cast<USDGameplayAbility>(AbilitySpec->Ability);
		AbilitySpec->SourceObject = this;
		if (AbilitySpec && Ability)
		{
			if (AbilitySpec->IsActive() && Ability->bActivateOnlyInputStart)
			{
				return;
			}

			ASDPlayerController* SDPlayerController = Cast<ASDPlayerController>(GetController());
			if (AbilitySystemComponent->TryActivateAbility(AbilitySpec->Handle))
			{
				SDPlayerController->SetCanReceiveRecentInputAction(false);
				SDPlayerController->ClearRecentInputAction();
			}
			else
			{
				SDPlayerController->SetRecentInputAction(Ability->AbilityInputAction);
			}
		}
	}
}

void ASDPlayerCharacter::HandleAbilityInputActionReleased(TSubclassOf<USDGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return;
	}

	if (USDAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent<USDAbilitySystemComponent>())
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass, true);
		if (AbilitySpec)
		{
			AbilitySystemComponent->AbilitySpecInputReleased(*AbilitySpec);
			AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec->Handle, AbilitySpec->ActivationInfo.GetActivationPredictionKey());

			if (ASDPlayerController* SDPlayerController = Cast<ASDPlayerController>(GetController()))
			{
				if (!SDPlayerController->CanReceiveRecentInputAction())
				{
					SDPlayerController->SetCanReceiveRecentInputAction(true);
					SDPlayerController->ClearRecentInputAction();
				}
			}
		}
	}
}

void ASDPlayerCharacter::MoveForward(float AxisValue)
{
	FGameplayTagContainer OwnedGamplayTag;
	GetOwnedGameplayTags(OwnedGamplayTag);
	FGameplayTag ImmovableTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Immovable"));
	FGameplayTag StunTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun"));
	if (OwnedGamplayTag.HasTag(ImmovableTag) || OwnedGamplayTag.HasTag(StunTag))
	{
		return;
	}

	FVector Direction = AxisValue * FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X);
	Direction.Z = 0.0f;
	Direction.Normalize();
	AddMovementInput(Direction);
}

void ASDPlayerCharacter::MoveRight(float AxisValue)
{
	FGameplayTagContainer OwnedGamplayTag;
	GetOwnedGameplayTags(OwnedGamplayTag);
	FGameplayTag ImmovableTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Immovable"));
	FGameplayTag StunTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun"));
	if (OwnedGamplayTag.HasTag(ImmovableTag) || OwnedGamplayTag.HasTag(StunTag))
	{
		return;
	}

	FVector Direction = AxisValue * FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
	Direction.Z = 0.0f;
	Direction.Normalize();
	AddMovementInput(Direction);
}

void ASDPlayerCharacter::Turn(float AxisValue)
{
	float TurnRate = AxisValue;
	float TurnRateDivisor = 1.0f;
	if (Camera)
	{
		TurnRateDivisor = 90.0f / Camera->FieldOfView;
	}

	AddControllerYawInput(TurnRate / TurnRateDivisor);
}

void ASDPlayerCharacter::LookUp(float AxisValue)
{
	float TurnRate = AxisValue;
	float TurnRateDivisor = 1.0f;
	if (Camera)
	{
		TurnRateDivisor = 90.0f / Camera->FieldOfView;
	}


	AddControllerPitchInput(TurnRate / TurnRateDivisor);
}
