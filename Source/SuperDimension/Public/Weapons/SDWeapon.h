// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SDWeapon.generated.h"

UCLASS()
class SUPERDIMENSION_API ASDWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetWeaponCollisionCenterLocationOffset();

protected:
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponCollisionCenterSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float WeaponCollisionRadius;

};
