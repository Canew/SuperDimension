// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SDProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "SDCharacter.h"

// Sets default values
ASDProjectile::ASDProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	ProjectileCollision->SetCollisionProfileName(TEXT("Projectile"));
	SetRootComponent(ProjectileCollision);

	ProjectileMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	InitialLifeSpan = 15.0f;
}

// Called when the game starts or when spawned
void ASDProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileCollision)
	{
		ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &ASDProjectile::OnProjectileCollisionBeginOverlap);
	}
}

// Called every frame
void ASDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UProjectileMovementComponent* ASDProjectile::GetProjectileMovement() const
{
	return ProjectileMovement;
}

void ASDProjectile::OnProjectileCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner() || OtherActor == this)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bool bFriendly = false;
	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	UAbilitySystemComponent* OtherAbilitySystenComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	IGenericTeamAgentInterface* SourceGenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (SourceGenericTeamAgentInterface)
	{
		ETeamAttitude::Type TeamAttitude = SourceGenericTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor);;
		switch (TeamAttitude)
		{
		case ETeamAttitude::Friendly:
			bFriendly = true;
			break;

		case ETeamAttitude::Hostile:
			break;

		case ETeamAttitude::Neutral:
			break;
		}
	}

	if (OwnerAbilitySystemComponent)
	{
		FGameplayEffectContextHandle GameplayEffectContext = OwnerAbilitySystemComponent->MakeEffectContext();
		GameplayEffectContext.AddHitResult(SweepResult, true);
		GameplayEffectContext.AddSourceObject(this);
		if (!bFriendly && OtherAbilitySystenComponent)
		{
			for (TSubclassOf<UGameplayEffect> ProjectileGameplayEffectClass : ProjectileGameplayEffectClasses)
			{
				OwnerAbilitySystemComponent->ApplyGameplayEffectToTarget(ProjectileGameplayEffectClass.GetDefaultObject(), OtherAbilitySystenComponent, UGameplayEffect::INVALID_LEVEL, GameplayEffectContext);
			}

			if (ASDCharacter* OtherCharacter = Cast<ASDCharacter>(OtherActor))
			{
				if (PushbackPower > 0.0f)
				{
					FVector LaunchDirection = ProjectileMovement->Velocity.GetSafeNormal2D();
					float LaunchPower = PushbackPower;
					OtherCharacter->Pushback(GetOwner(), LaunchPower * LaunchDirection, PushbackStunDuration);
				}
			}
		}

		if (ProjectileImpactGameplayCueTag.IsValid())
		{
			FGameplayCueParameters GameplayCueParameters;
			GameplayCueParameters.EffectCauser = GetOwner();
			GameplayCueParameters.EffectContext = GameplayEffectContext;
			GameplayCueParameters.PhysicalMaterial = SweepResult.PhysMaterial;
			OtherAbilitySystenComponent
				? OtherAbilitySystenComponent->ExecuteGameplayCue(ProjectileImpactGameplayCueTag, GameplayCueParameters)
				: OwnerAbilitySystemComponent->ExecuteGameplayCue(ProjectileImpactGameplayCueTag, GameplayCueParameters);
		}
	}

	Destroy();
}

