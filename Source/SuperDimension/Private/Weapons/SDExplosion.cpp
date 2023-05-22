


#include "Weapons/SDExplosion.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "SDCharacter.h"

// Sets default values
ASDExplosion::ASDExplosion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosionCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetCollisionProfileName(TEXT("Weapon"));
	SetRootComponent(ExplosionCollision);

	InitialLifeSpan = 15.0f;
}

// Called when the game starts or when spawned
void ASDExplosion::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASDExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDExplosion::BeginExplosionCollisionOverlap()
{
	if (ExplosionCollision)
	{
		ExplosionCollision->OnComponentBeginOverlap.AddDynamic(this, &ASDExplosion::OnExplosionCollisionBeginOverlap);

		TArray<FHitResult> HitResults;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		GetWorld()->SweepMultiByProfile(HitResults,
			ExplosionCollision->GetComponentLocation(),
			ExplosionCollision->GetComponentLocation() + FVector(0.0f, 0.0f, 0.01f),
			GetActorRotation().Quaternion(),
			ExplosionCollision->GetCollisionProfileName(),
			FCollisionShape::MakeCapsule(ExplosionCollision->GetScaledCapsuleHalfHeight(),
			ExplosionCollision->GetScaledCapsuleRadius()),
			CollisionQueryParams);

		for (const FHitResult& HitResult : HitResults)
		{
			AActor* OtherActor = HitResult.GetActor();
			if (!OtherActor || AffectedActors.Find(OtherActor) != INDEX_NONE)
			{
				continue;
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
				GameplayEffectContext.AddHitResult(HitResult, true);
				GameplayEffectContext.AddSourceObject(this);
				if (!bFriendly && OtherAbilitySystenComponent)
				{
					for (TSubclassOf<UGameplayEffect> ProjectileGameplayEffectClass : ExplosionGameplayEffectClasses)
					{
						AffectedActors.Add(OtherActor);
						OwnerAbilitySystemComponent->ApplyGameplayEffectToTarget(ProjectileGameplayEffectClass.GetDefaultObject(), OtherAbilitySystenComponent, UGameplayEffect::INVALID_LEVEL, GameplayEffectContext);
					}

					if (ASDCharacter* OtherCharacter = Cast<ASDCharacter>(OtherActor))
					{
						if (PushbackPower > 0.0f)
						{
							FVector LaunchDirection = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
							float LaunchPower = PushbackPower;
							OtherCharacter->Pushback(GetOwner(), LaunchPower * LaunchDirection, PushbackStunDuration);
						}
					}
				}

				if (ExplosionImpactGameplayCueTag.IsValid())
				{
					FGameplayCueParameters GameplayCueParameters;
					GameplayCueParameters.EffectCauser = GetOwner();
					GameplayCueParameters.EffectContext = GameplayEffectContext;
					GameplayCueParameters.PhysicalMaterial = HitResult.PhysMaterial;
					OtherAbilitySystenComponent
						? OtherAbilitySystenComponent->ExecuteGameplayCue(ExplosionImpactGameplayCueTag, GameplayCueParameters)
						: OwnerAbilitySystemComponent->ExecuteGameplayCue(ExplosionImpactGameplayCueTag, GameplayCueParameters);
				}
			}
		}

		FTimerHandle EndOverlapTimerHandle;
		GetWorldTimerManager().SetTimer(EndOverlapTimerHandle, this, &ASDExplosion::EndExplosionCollisionOverlap, OverlapDuration, false);
	}
}

void ASDExplosion::EndExplosionCollisionOverlap()
{
	if (ExplosionCollision)
	{
		ExplosionCollision->OnComponentBeginOverlap.RemoveDynamic(this, &ASDExplosion::OnExplosionCollisionBeginOverlap);
	}
}

void ASDExplosion::OnExplosionCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == this || AffectedActors.Find(OtherActor) != INDEX_NONE)
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
			for (TSubclassOf<UGameplayEffect> ProjectileGameplayEffectClass : ExplosionGameplayEffectClasses)
			{
				AffectedActors.Add(OtherActor);
				OwnerAbilitySystemComponent->ApplyGameplayEffectToTarget(ProjectileGameplayEffectClass.GetDefaultObject(), OtherAbilitySystenComponent, UGameplayEffect::INVALID_LEVEL, GameplayEffectContext);
			}

			if (ASDCharacter* OtherCharacter = Cast<ASDCharacter>(OtherActor))
			{
				if (PushbackPower > 0.0f)
				{
					FVector LaunchDirection = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
					float LaunchPower = PushbackPower;
					OtherCharacter->Pushback(GetOwner(), LaunchPower * LaunchDirection, PushbackStunDuration);
				}
			}
		}

		if (ExplosionImpactGameplayCueTag.IsValid())
		{
			FGameplayCueParameters GameplayCueParameters;
			GameplayCueParameters.EffectCauser = GetOwner();
			GameplayCueParameters.EffectContext = GameplayEffectContext;
			GameplayCueParameters.PhysicalMaterial = SweepResult.PhysMaterial;
			OtherAbilitySystenComponent 
				? OtherAbilitySystenComponent->ExecuteGameplayCue(ExplosionImpactGameplayCueTag, GameplayCueParameters)
				: OwnerAbilitySystemComponent->ExecuteGameplayCue(ExplosionImpactGameplayCueTag, GameplayCueParameters);
		}
	}
}


