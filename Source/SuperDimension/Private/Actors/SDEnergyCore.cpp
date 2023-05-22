


#include "Actors/SDEnergyCore.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UMG/SDInteractWidget.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASDEnergyCore::ASDEnergyCore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnergyCoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnergyCoreMesh"));
	SetRootComponent(EnergyCoreMesh);

	EnergyCoreParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EnergyCoreParticle"));
	EnergyCoreParticle->SetupAttachment(GetRootComponent());

	bActivated = false;
	EnergyCoreBrightness = -1.0f;
}

// Called when the game starts or when spawned
void ASDEnergyCore::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void ASDEnergyCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FText ASDEnergyCore::GetInteractionDescription() const
{
	return FText::FromString(TEXT("Activate"));
}

bool ASDEnergyCore::CanInteract() const
{
	return true;
}

void ASDEnergyCore::Interact()
{
	ActivateEnergyCore();
}

void ASDEnergyCore::ActivateEnergyCore()
{
	bActivated = true;

	EnergyCoreMaterialInstanceDynamic = EnergyCoreMesh->CreateDynamicMaterialInstance(1);
	if (EnergyCoreMaterialInstanceDynamic)
	{
		GetWorldTimerManager().SetTimer(TickBrightnessTimerHandle, this, &ASDEnergyCore::TickEnergyCoreBrightness, 0.05f, true);
	}

	if (EnergyCoreParticle)
	{
		EnergyCoreParticle->SetColorParameter(TEXT("Color"), FLinearColor(150.0f, 30.0f, 1.5f));
	}
}

void ASDEnergyCore::TickEnergyCoreBrightness()
{
	if (EnergyCoreMaterialInstanceDynamic)
	{
		EnergyCoreBrightness += 0.02f;
		EnergyCoreMaterialInstanceDynamic->SetScalarParameterValue(TEXT("Brightness"), EnergyCoreBrightness);
		if (EnergyCoreBrightness > 4.0f)
		{
			GetWorldTimerManager().ClearTimer(TickBrightnessTimerHandle);
		}
	}
}

