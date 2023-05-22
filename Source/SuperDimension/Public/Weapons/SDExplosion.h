

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SDExplosion.generated.h"

UCLASS()
class SUPERDIMENSION_API ASDExplosion : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASDExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginExplosionCollisionOverlap();
	void EndExplosionCollisionOverlap();

protected:
	UFUNCTION()
	void OnExplosionCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* ExplosionCollision;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	TArray<TSubclassOf<class UGameplayEffect>> ExplosionGameplayEffectClasses;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	float PushbackPower;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	float PushbackStunDuration;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag ExplosionImpactGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float OverlapDuration;

	TArray<AActor*> AffectedActors;
};
