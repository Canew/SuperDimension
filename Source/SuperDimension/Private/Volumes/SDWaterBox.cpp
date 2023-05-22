


#include "Volumes/SDWaterBox.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SDCharacterMovementComponent.h"

// Sets default values
ASDWaterBox::ASDWaterBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WaterBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Water Box"));
	WaterBox->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	SetRootComponent(WaterBox);

	WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water Mesh"));
	WaterMesh->SetRelativeLocation(FVector(0.0f, 0.0f, WaterBox->GetUnscaledBoxExtent().Z));
	WaterMesh->SetupAttachment(WaterBox);

	WaterPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Water Post Process"));
	WaterPostProcess->bUnbound = false;
	WaterPostProcess->SetupAttachment(WaterBox);
}

// Called when the game starts or when spawned
void ASDWaterBox::BeginPlay()
{
	Super::BeginPlay();
	
	if (WaterBox)
	{
		WaterBox->OnComponentBeginOverlap.AddDynamic(this, &ASDWaterBox::OnBoxComponentBeginOverlap);
		WaterBox->OnComponentEndOverlap.AddDynamic(this, &ASDWaterBox::OnBoxComponentEndOverlap);
	}
}

// Called every frame
void ASDWaterBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDWaterBox::OnBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor))
	{
		if (USDCharacterMovementComponent* CharacterMovementComponent = Cast<USDCharacterMovementComponent>(OtherCharacter->GetCharacterMovement()))
		{
			CharacterMovementComponent->EnterWater(WaterMesh->GetComponentLocation().Z);
		}
	}
}

void ASDWaterBox::OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor))
	{
		if (USDCharacterMovementComponent* CharacterMovementComponent = Cast<USDCharacterMovementComponent>(OtherCharacter->GetCharacterMovement()))
		{
			CharacterMovementComponent->ExitWater();
		}
	}
}
