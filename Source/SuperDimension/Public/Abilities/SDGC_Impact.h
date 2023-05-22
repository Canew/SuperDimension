// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraSystem.h"
#include "SDGC_Impact.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FGameplayCueEffectContainer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MarkMaterial;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* Particle;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* Niagara;

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* Sound;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> CameraShake;
};

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGC_Impact : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

private:
	virtual UPhysicalMaterial* GetHitPhysicalMaterial(UPrimitiveComponent* HitComponent);

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<TEnumAsByte<EPhysicalSurface>, FGameplayCueEffectContainer> ImpactGameplayCueEffects;
};
