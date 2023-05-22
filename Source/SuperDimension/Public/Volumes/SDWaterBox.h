

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SDWaterBox.generated.h"

UCLASS()
class SUPERDIMENSION_API ASDWaterBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDWaterBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* WaterBox;

	UPROPERTY(VisibleAnywhere)
	class UPostProcessComponent* WaterPostProcess;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WaterMesh;
};
