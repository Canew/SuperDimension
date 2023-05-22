// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystemComponent.h"
#include "GameplayInventory.h"
#include "GameplayItem.h"
#include "InventorySystemInterface.h"
#include "Abilities/GameplayAbility.h"

// Sets default values for this component's properties
UInventorySystemComponent::UInventorySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}


// Called when the game starts
void UInventorySystemComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInventorySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Init starting inventory
	for (int32 i = 0; i < InitialInventoryData.Num(); ++i)
	{
		if (InitialInventoryData[i].MaxRow > 0 && InitialInventoryData[i].MaxColumn > 0)
		{
			UGameplayInventory* NewInventory = UGameplayInventory::CreateInventory(this, InitialInventoryData[i].MaxRow, InitialInventoryData[i].MaxColumn, InitialInventoryData[i].DefaultAccessPermission);
			if (NewInventory)
			{
				SpawnedInventories.Add(NewInventory);
			}
		}
	}
}

// Called every frame
void UInventorySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventorySystemComponent::RegisterInventory(UGameplayInventory* TargetInventory)
{
	if (TargetInventory && !SpawnedInventories.Contains(TargetInventory))
	{
		SpawnedInventories.Add(TargetInventory);
	}
}

EAccessPermission UInventorySystemComponent::GetExternalAccessPermission(UGameplayInventory* TargetInventory) const
{
	if (TargetInventory == nullptr)
	{
		return EAccessPermission::Denied;
	}

	// return full permission if inventory owner
	if (SpawnedInventories.Contains(TargetInventory))
	{
		return EAccessPermission::ReadWrite;
	}
	else
	{
		return TargetInventory->GetExternalAccessPermission();
	}
}

int32 UInventorySystemComponent::GetItemCount(TSubclassOf<UGameplayItem> ItemClass) const
{
	int32 TotalItemCount = 0;
	for (UGameplayInventory* SpawnedInventory : SpawnedInventories)
	{
		if (GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadOnly || GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadWrite)
		{
			TotalItemCount += SpawnedInventory->GetItemCount(ItemClass);
		}
	}

	return TotalItemCount;
}

int32 UInventorySystemComponent::GetItemCountAtInventory(UGameplayInventory* TargetInventory, TSubclassOf<UGameplayItem> ItemClass) const
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadOnly || GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->GetItemCount(ItemClass);
	}

	return false;
}

UGameplayItem* UInventorySystemComponent::GetItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex) const
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadOnly || GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->GetItemAtIndex(InventoryIndex);
	}

	return false;
}

int32 UInventorySystemComponent::GetItemCountAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex) const
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadOnly || GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->GetItemCountAtIndex(InventoryIndex);
	}

	return false;
}

int32 UInventorySystemComponent::AddItem(UGameplayItem* ItemToAdd, int32 ItemCountToAdd)
{
	if (ItemToAdd == nullptr)
	{
		return ItemCountToAdd;
	}

	int32 LeftItemCount = ItemCountToAdd;
	for (UGameplayInventory* SpawnedInventory : SpawnedInventories)
	{
		if (LeftItemCount <= 0)
		{
			break;
		}

		if (GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadWrite)
		{
			LeftItemCount = SpawnedInventory->AddItem(ItemToAdd, LeftItemCount, false);
		}
	}

	if (LeftItemCount > 0)
	{
		for (UGameplayInventory* SpawnedInventory : SpawnedInventories)
		{
			if (LeftItemCount <= 0)
			{
				break;
			}

			if (GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadWrite)
			{
				LeftItemCount = SpawnedInventory->AddItem(ItemToAdd, LeftItemCount);
			}
		}
	}

	return LeftItemCount;
}

int32 UInventorySystemComponent::AddItemByClass(TSubclassOf<UGameplayItem> ItemClassToAdd, int32 ItemCountToAdd)
{
	if (ItemClassToAdd == nullptr)
	{
		return ItemCountToAdd;
	}

	UGameplayItem* NewItem = NewObject<UGameplayItem>(this, ItemClassToAdd);
	if (NewItem == nullptr)
	{
		return ItemCountToAdd;
	}

	int32 LeftItemCount = ItemCountToAdd;
	for (UGameplayInventory* SpawnedInventory : SpawnedInventories)
	{
		if (LeftItemCount <= 0)
		{
			break;
		}

		if (GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadWrite)
		{
			LeftItemCount = SpawnedInventory->AddItem(NewItem, LeftItemCount, false);
		}
	}

	if (LeftItemCount > 0)
	{
		for (UGameplayInventory* SpawnedInventory : SpawnedInventories)
		{
			if (LeftItemCount <= 0)
			{
				break;
			}

			if (GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadWrite)
			{
				LeftItemCount = SpawnedInventory->AddItem(NewItem, LeftItemCount);
			}
		}
	}

	return LeftItemCount;
}

int32 UInventorySystemComponent::AddItemAtInventory(UGameplayInventory* TargetInventory, UGameplayItem* ItemToAdd, int32 ItemCountToAdd)
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->AddItem(ItemToAdd, ItemCountToAdd, true);
	}

	return ItemCountToAdd;
}

int32 UInventorySystemComponent::AddItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex, UGameplayItem* ItemToAdd, int32 ItemCountToAdd)
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->AddItemAtIndex(InventoryIndex, ItemToAdd, ItemCountToAdd);
	}

	return ItemCountToAdd;
}

int32 UInventorySystemComponent::RemoveItem(UGameplayItem* ItemToRemove, int32 ItemCountToRemove)
{
	int32 LeftItemCount = ItemCountToRemove;
	for (UGameplayInventory* SpawnedInventory : SpawnedInventories)
	{
		if (LeftItemCount <= 0)
		{
			break;
		}

		if (GetExternalAccessPermission(SpawnedInventory) == EAccessPermission::ReadWrite)
		{
			LeftItemCount = SpawnedInventory->RemoveItem(ItemToRemove, LeftItemCount);
		}
	}

	return LeftItemCount;
}

int32 UInventorySystemComponent::RemoveItemAtInventory(UGameplayInventory* TargetInventory, UGameplayItem* ItemToRemove, int32 ItemCountToRemove)
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->RemoveItem(ItemToRemove, ItemCountToRemove);
	}

	return ItemCountToRemove;
}

int32 UInventorySystemComponent::RemoveItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex, int32 ItemCountToRemove)
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->RemoveItemAtIndex(InventoryIndex, ItemCountToRemove);
	}

	return ItemCountToRemove;
}

void UInventorySystemComponent::SwapItemPosition(UGameplayInventory* TargetInventoryA, int32 InventoryIndexA, UGameplayInventory* TargetInventoryB, int32 InventoryIndexB)
{
	if (TargetInventoryA == TargetInventoryB)
	{
		if (GetExternalAccessPermission(TargetInventoryA) == EAccessPermission::ReadWrite)
		{
			return TargetInventoryA->SwapItemPosition(InventoryIndexA, InventoryIndexB);
		}
	}
	else
	{
		if (GetExternalAccessPermission(TargetInventoryA) == EAccessPermission::ReadWrite && GetExternalAccessPermission(TargetInventoryB) == EAccessPermission::ReadWrite)
		{
			UGameplayItem* ItemA = TargetInventoryA->GetItemAtIndex(InventoryIndexA);
			int32 ItemCountA = TargetInventoryA->GetItemCountAtIndex(InventoryIndexA);;

			UGameplayItem* ItemB = TargetInventoryB->GetItemAtIndex(InventoryIndexB);
			int32 ItemCountB = TargetInventoryB->GetItemCountAtIndex(InventoryIndexB);

			TargetInventoryA->RemoveItemAtIndex(InventoryIndexA, ItemCountA);
			TargetInventoryA->AddItemAtIndex(InventoryIndexA, ItemB, ItemCountB);

			TargetInventoryB->RemoveItemAtIndex(InventoryIndexB, ItemCountB);	
			TargetInventoryB->AddItemAtIndex(InventoryIndexB, ItemA, ItemCountA);
		}
	}
}

bool UInventorySystemComponent::UseItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex)
{
	if (GetExternalAccessPermission(TargetInventory) == EAccessPermission::ReadWrite)
	{
		return TargetInventory->UseItem(InventoryIndex);
	}

	return false;
}

UInventorySystemComponent* UInventorySystemComponent::GetInventorySystemComponentFromActor(AActor* Actor, bool LookForComponent)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	const IInventorySystemInterface* InventorySystemInterface = Cast<IInventorySystemInterface>(Actor);
	if (InventorySystemInterface)
	{
		return InventorySystemInterface->GetInventorySystemComponent();
	}

	if (LookForComponent)
	{
		/** This is slow and not desirable */
		UE_LOG(LogTemp, Warning, TEXT("GetInventorySystemComponentFromActor called on %s that is not IInventorySystemInterface. This slow!"), *Actor->GetName());

		return Actor->FindComponentByClass<UInventorySystemComponent>();
	}

	return nullptr;
}

TArray<UGameplayInventory*> UInventorySystemComponent::GetSpawnedInventories() const
{
	return SpawnedInventories;
}
