// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SDWeapon.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASDWeapon::ASDWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

// Called when the game starts or when spawned
void ASDWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

FVector ASDWeapon::GetWeaponCollisionCenterLocationOffset()
{
	if (AActor* ParentActor = GetAttachParentActor())
	{
		if (USkeletalMeshComponent* ParentSkeltalMeshComponent = ParentActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (WeaponMesh->DoesSocketExist(WeaponCollisionCenterSocketName))
			{
				FTransform SocketTransform = WeaponMesh->GetSocketTransform(WeaponCollisionCenterSocketName, ERelativeTransformSpace::RTS_Component);
				return SocketTransform.GetLocation();
			}
		}
	}

	return FVector::ZeroVector;
}

