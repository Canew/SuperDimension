// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SDProjectile.generated.h"

UCLASS()
class SUPERDIMENSION_API ASDProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class UProjectileMovementComponent* GetProjectileMovement() const;

protected:
	UFUNCTION()
	void OnProjectileCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* ProjectileCollision;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	TArray<TSubclassOf<class UGameplayEffect>> ProjectileGameplayEffectClasses;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	float PushbackPower;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	float PushbackStunDuration;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag ProjectileImpactGameplayCueTag;
};
