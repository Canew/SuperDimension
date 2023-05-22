

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SDInteractionInterface.h"
#include "SDEnergyCore.generated.h"

UCLASS()
class SUPERDIMENSION_API ASDEnergyCore : public AActor, public ISDInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDEnergyCore();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual FText GetInteractionDescription() const override;

	virtual bool CanInteract() const override;

	virtual void Interact() override;

protected:
	virtual void ActivateEnergyCore();
	virtual void TickEnergyCoreBrightness();

protected:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* EnergyCoreMesh;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* EnergyCoreParticle;

	bool bActivated;
	class UMaterialInstanceDynamic* EnergyCoreMaterialInstanceDynamic;
	FTimerHandle TickBrightnessTimerHandle;
	float EnergyCoreBrightness;
};
