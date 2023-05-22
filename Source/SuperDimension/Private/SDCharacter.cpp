// Fill out your copyright notice in the Description page of Project Settings.


#include "SDCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SDCharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "Abilities/SDGameplayAbility.h"
#include "Abilities/SDGE_Stun.h"
#include "AI/SDAIController.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Weapons/SDWeapon.h"
#include "SDPlayerState.h"

// Sets default values
ASDCharacter::ASDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->bReturnMaterialOnMove = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bNetworkMovementModeChanged = true;
	bUseControllerRotationYaw = false;

	AIControllerClass = ASDAIController::StaticClass();
	
	// Perception
	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSource"));

	HardLandingVelocityZ = -800.0f;
}

// Called every frame
void ASDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void ASDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASDCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerState* PS = GetPlayerState<APlayerState>())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		InitializeDefaultAbilities();
		InitializeDefaultAttributes();

		GetAbilitySystemComponent()->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ASDCharacter::HandleGameplayEffectExecuteDelegateOnSelf);
	}
	else
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	}

	Crouch();
}

void ASDCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (GetCharacterMovement()->Velocity.Z < HardLandingVelocityZ)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
		{
			FGameplayEventData GameplayEventData;
			AbilitySystemComponent->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(TEXT("Event.Trigger.Landing")), &GameplayEventData);
		}
	}

}

// Called when the game starts or when spawned
void ASDCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementModeChangedDelegate.AddDynamic(this, &ASDCharacter::OnCharacterMovementModeChanged);
	AIPerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	EquipDefaultWeapon();
}

void ASDCharacter::GetOwnedGameplayTags(FGameplayTagContainer & TagContainer) const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
	}
}

UAbilitySystemComponent* ASDCharacter::GetAbilitySystemComponent() const
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetPlayerState());
	if (AbilitySystemInterface)
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}

	return nullptr;
}

UAttributeSet* ASDCharacter::GetAttributeSet() const
{
	if (ASDPlayerState* SDPlayerState = GetPlayerState<ASDPlayerState>())
	{
		return SDPlayerState->GetAttributeSet();
	}

	return nullptr;
}

void ASDCharacter::InitializeDefaultAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		for (const TSubclassOf<UGameplayAbility>& DefaultAbility : DefaultAbilities)
		{
			if (DefaultAbility)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
			}
		}
	}
}

void ASDCharacter::InitializeDefaultAttributes()
{
	// Run only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		for (TSubclassOf<UGameplayEffect> DefaultAttribute : DefaultAttributes)
		{
			FGameplayEffectSpecHandle AttributeEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttribute, 0, EffectContext);
			if (AttributeEffectSpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*AttributeEffectSpecHandle.Data.Get());
			}
		}
	}
}

void ASDCharacter::HandleGameplayEffectExecuteDelegateOnSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return;
	}

	FGameplayTagContainer GrantedGameplayTagContainer;
	SpecExecuted.GetAllGrantedTags(GrantedGameplayTagContainer);
	FGameplayTag StunTag = FGameplayTag::RequestGameplayTag(TEXT("Character.State.Stun"));
	if (GrantedGameplayTagContainer.HasTag(StunTag))
	{
		FGameplayTag StunImmuneTag = FGameplayTag::RequestGameplayTag(TEXT("Character.Immune.Stun"));
		if (!AbilitySystemComponent->HasMatchingGameplayTag(StunImmuneTag))
		{
			FGameplayTagContainer CancelWithTags;
			CancelWithTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability")));
			FGameplayTagContainer CancelWithoutTags;
			CancelWithoutTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Immune.Stun")));

			AbilitySystemComponent->CancelAbilities(&CancelWithTags, &CancelWithoutTags);
		}
	}
}

FName ASDCharacter::GetNickname() const
{
	return Nickname;
}

void ASDCharacter::EquipDefaultWeapon()
{
	EquippedWeapon = GetWorld()->SpawnActor<ASDWeapon>(DefaultWeaponClass);
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, DefaultWeaponSocketName);
	}
}

FGenericTeamId ASDCharacter::GetGenericTeamId() const
{
	if (IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return GenericTeamAgentInterface->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

void ASDCharacter::Pushback(AActor* PushInstigator, const FVector& PushBackVelocity, float PushBackStunDuration)
{
	UAbilitySystemComponent* InstigatorAbilitySystemComponent = nullptr;
	if (IAbilitySystemInterface* InstigatorAbilitySystemInterface = Cast<IAbilitySystemInterface>(PushInstigator))
	{
		InstigatorAbilitySystemComponent = InstigatorAbilitySystemInterface->GetAbilitySystemComponent();
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle StunEffectContextHandle = InstigatorAbilitySystemComponent ? InstigatorAbilitySystemComponent->MakeEffectContext() : AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle StunEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(USDGE_Stun::StaticClass(), UGameplayEffect::INVALID_LEVEL, StunEffectContextHandle);
		StunEffectSpecHandle.Data->SetDuration(PushBackStunDuration, true);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*StunEffectSpecHandle.Data);
	}

	FGameplayTagContainer OwnedGameplayTag;
	GetOwnedGameplayTags(OwnedGameplayTag);
	FGameplayTag PushBackImmuneTag = FGameplayTag::RequestGameplayTag(TEXT("Character.Immune.Pushback"));
	if (!OwnedGameplayTag.HasTag(PushBackImmuneTag))
	{
		FVector FinalPushBackVelocity = PushBackVelocity.GetSafeNormal2D();
		LaunchCharacter(PushBackVelocity, false, false);
	}
}

void ASDCharacter::OnCharacterMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	}
	else
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}
}
