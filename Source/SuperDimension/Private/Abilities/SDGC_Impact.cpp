// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SDGC_Impact.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

void USDGC_Impact::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	if (!HitResult || !HitResult->GetActor() || !HitResult->GetComponent())
	{
		return;
	}
	
	FVector HitResultLocation = HitResult->Location;
	if (HitResultLocation.IsNearlyZero())
	{
		HitResultLocation = HitResult->GetActor()->GetActorLocation();
	}

	FVector HitResultImpactPoint = HitResult->ImpactPoint;
	if (HitResultImpactPoint.IsNearlyZero())
	{
		HitResultImpactPoint = HitResult->GetActor()->GetActorLocation();
	}

	UPhysicalMaterial* HitPhysicalMaterial = GetHitPhysicalMaterial(HitResult->GetComponent());
	FGameplayCueEffectContainer* GameplayCueEffectContainer = nullptr;
	if (HitPhysicalMaterial)
	{
		EPhysicalSurface PhysicalSurface = HitPhysicalMaterial->SurfaceType.GetValue();
		GameplayCueEffectContainer = ImpactGameplayCueEffects.Find(PhysicalSurface);
	}

	if (!GameplayCueEffectContainer)
	{
		GameplayCueEffectContainer = ImpactGameplayCueEffects.Find(EPhysicalSurface::SurfaceType_Default);
		if (!GameplayCueEffectContainer)
		{
			return;
		}
	}

	UMaterialInterface* MarkMaterial = GameplayCueEffectContainer->MarkMaterial;
	UParticleSystem* Particle = GameplayCueEffectContainer->Particle;
	UNiagaraSystem* Niagara = GameplayCueEffectContainer->Niagara;
	USoundBase* Sound = GameplayCueEffectContainer->Sound;
	TSubclassOf<UCameraShakeBase> CameraShake = GameplayCueEffectContainer->CameraShake;

	// Spawn mark decal
	if (MarkMaterial)
	{
		FRotator ProjectileMarkRotator = UKismetMathLibrary::MakeRotFromX(HitResult->Normal);
		ProjectileMarkRotator.Roll = FMath::RandRange(0.0f, 360.0f);
		UDecalComponent* ProjectileMark = UGameplayStatics::SpawnDecalAttached(
			MarkMaterial,
			FVector(5.0f, 2.0f, 2.0f),
			HitResult->GetComponent(),
			NAME_None,
			HitResultImpactPoint,
			ProjectileMarkRotator,
			EAttachLocation::KeepWorldPosition,
			5.0f);

		if (ProjectileMark)
		{
			ProjectileMark->SetFadeScreenSize(0.005f);
		}
	}

	// Spawn particle
	FRotator HitResultNormalRotator = UKismetMathLibrary::MakeRotFromZ(HitResult->Normal);
	if (Particle)
	{
		UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
			Particle,
			HitResult->GetComponent(),
			NAME_None,
			HitResultImpactPoint,
			HitResultNormalRotator,
			EAttachLocation::KeepWorldPosition);
	}

	// Spawn niagara
	if (Niagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			Niagara,
			HitResult->GetComponent(),
			NAME_None,
			HitResultImpactPoint,
			HitResultNormalRotator,
			EAttachLocation::KeepWorldPosition,
			true);
	}

	// Play sound
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			MyTarget,
			Sound,
			HitResultImpactPoint);
	}

	if (CameraShake)
	{
		// Play camera shake
		APawn* SourcePawn = Cast<APawn>(Parameters.GetEffectCauser());
		if (SourcePawn)
		{
			APlayerController* SourcePlayerController = Cast<APlayerController>(SourcePawn->GetController());
			if (SourcePlayerController)
			{
				SourcePlayerController->ClientStartCameraShake(CameraShake);
			}
		}

		APawn* TargetPawn = Cast<APawn>(MyTarget);
		if (TargetPawn)
		{
			APlayerController* TargetPlayerController = Cast<APlayerController>(TargetPawn->GetController());
			if (TargetPlayerController)
			{
				TargetPlayerController->ClientStartCameraShake(CameraShake);
			}
		}
	}
}

UPhysicalMaterial* USDGC_Impact::GetHitPhysicalMaterial(UPrimitiveComponent* HitComponent)
{
	if (HitComponent)
	{
		return HitComponent->BodyInstance.GetSimplePhysicalMaterial();
	}

	return nullptr;
}

