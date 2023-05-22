#include "GameplayInventory.h"
#include "InventorySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

int32 UGameplayInventory::GetItemCount(TSubclassOf<UGameplayItem> ItemClass) const
{
	if (ItemClass == nullptr)
	{
		return 0;
	}

	int32 ItemCount = 0;
	for (auto Iterator = InventoryDataMap.CreateConstIterator(); Iterator; ++Iterator)
	{
		if (Iterator.Value().SlotItem && Iterator.Value().SlotItem->GetClass() == ItemClass)
		{
			ItemCount += Iterator.Value().SlotItemCount;
		}
	}

	return ItemCount;
}

UGameplayItem* UGameplayInventory::GetItemAtIndex(int32 InventoryIndex) const
{
	const FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData)
	{
		return InventorySlotData->SlotItem;
	}

	return nullptr;
}

int32 UGameplayInventory::GetItemCountAtIndex(int32 InventoryIndex) const
{
	const FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData)
	{
		return InventorySlotData->SlotItemCount;
	}

	return 0;
}

int32 UGameplayInventory::AddItemByClass(TSubclassOf<UGameplayItem> ItemClassToAdd, int32 ItemCountToAdd, bool bFillLeftItemToEmptySlot)
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

	InventoryDataMap.KeySort([](int32 A, int32 B) { return A < B; });

	if (NewItem->IsStackable())
	{
		// Add Item to existing item in inventory
		for (auto Iterator = InventoryDataMap.CreateIterator(); Iterator; ++Iterator)
		{
			if (ItemCountToAdd <= 0)
			{
				break;
			}

			if (Iterator.Value().SlotItem->GetClass() == ItemClassToAdd)
			{
				int32 MaxAddibleCount =
					Iterator.Value().SlotItem->GetMaxStackCount() == 0
					? INT_MAX
					: Iterator.Value().SlotItem->GetMaxStackCount() - Iterator.Value().SlotItemCount;
				if (ItemCountToAdd > MaxAddibleCount)
				{
					InventoryDataMap[Iterator.Key()].SlotItemCount += MaxAddibleCount;
					OnInventorySlotUpdated.Broadcast(this, Iterator.Key(), NewItem, InventoryDataMap[Iterator.Key()].SlotItemCount);
					ItemCountToAdd -= MaxAddibleCount;
				}
				else
				{
					InventoryDataMap[Iterator.Key()].SlotItemCount += ItemCountToAdd;
					OnInventorySlotUpdated.Broadcast(this, Iterator.Key(), NewItem, InventoryDataMap[Iterator.Key()].SlotItemCount);
					ItemCountToAdd = 0;
				}
			}
		}

	}
	
	// Fill item to empty slot if count to add remains. 
	if (bFillLeftItemToEmptySlot && ItemCountToAdd > 0)
	{
		ItemCountToAdd = FillItemToEmptySlot(NewItem, ItemCountToAdd);
	}

	return ItemCountToAdd;
}

int32 UGameplayInventory::AddItem(UGameplayItem* ItemToAdd, int32 ItemCountToAdd, bool bFillLeftItemToEmptySlot)
{
	if (ItemToAdd == nullptr)
	{
		return ItemCountToAdd;
	}

	InventoryDataMap.KeySort([](int32 A, int32 B) { return A < B; });

	if (ItemToAdd->IsStackable())
	{
		// Add Item to existing item in inventory
		for (auto Iterator = InventoryDataMap.CreateIterator(); Iterator; ++Iterator)
		{
			if (ItemCountToAdd <= 0)
			{
				break;
			}

			if (Iterator.Value().SlotItem->GetClass() == ItemToAdd->GetClass())
			{
				int32 MaxAddibleCount =
					Iterator.Value().SlotItem->GetMaxStackCount() == 0
					? INT_MAX
					: Iterator.Value().SlotItem->GetMaxStackCount() - Iterator.Value().SlotItemCount;
				if (ItemCountToAdd > MaxAddibleCount)
				{
					InventoryDataMap[Iterator.Key()].SlotItemCount += MaxAddibleCount;
					OnInventorySlotUpdated.Broadcast(this, Iterator.Key(), ItemToAdd, InventoryDataMap[Iterator.Key()].SlotItemCount);
					ItemCountToAdd -= MaxAddibleCount;
				}
				else
				{
					InventoryDataMap[Iterator.Key()].SlotItemCount += ItemCountToAdd;
					OnInventorySlotUpdated.Broadcast(this, Iterator.Key(), ItemToAdd, InventoryDataMap[Iterator.Key()].SlotItemCount);
					ItemCountToAdd = 0;
				}
			}
		}

	}

	// Fill item to empty slot if count to add remains. 
	if (bFillLeftItemToEmptySlot && ItemCountToAdd > 0)
	{
		ItemCountToAdd = FillItemToEmptySlot(ItemToAdd, ItemCountToAdd);
	}

	return ItemCountToAdd;
}

int32 UGameplayInventory::AddItemAtIndex(int32 InventoryIndex, UGameplayItem* ItemToAdd, int32 ItemCountToAdd)
{
	if (ItemToAdd == nullptr || InventoryIndex < 0 || InventoryIndex > GetMaxIndex())
	{
		return ItemCountToAdd;
	}

	if (ItemCountToAdd <= 0)
	{
		return 0;
	}

	FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData)
	{
		if (!InventorySlotData->SlotItem->IsStackable() || ItemToAdd->StaticClass() != InventorySlotData->SlotItem->StaticClass())
		{
			return ItemCountToAdd;
		}

		int32 AddibleCount =
			InventorySlotData->SlotItem->GetMaxStackCount() == 0
			? INT_MAX
			: InventorySlotData->SlotItem->GetMaxStackCount() - InventorySlotData->SlotItemCount;
		if (ItemCountToAdd > AddibleCount)
		{
			InventorySlotData->SlotItemCount += AddibleCount;
			OnInventorySlotUpdated.Broadcast(this, InventoryIndex, InventorySlotData->SlotItem, InventorySlotData->SlotItemCount);
			ItemCountToAdd -= AddibleCount;
		}
		else
		{
			InventorySlotData->SlotItemCount += ItemCountToAdd;
			OnInventorySlotUpdated.Broadcast(this, InventoryIndex, InventorySlotData->SlotItem, InventorySlotData->SlotItemCount);
			ItemCountToAdd = 0;
		}
	}
	else
	{
		int32 MaxStackCount = ItemToAdd->GetMaxStackCount() == 0 ? INT_MAX : ItemToAdd->GetMaxStackCount();
		int32 AddibleCount = FMath::Clamp(ItemCountToAdd, 0, MaxStackCount);
		InventoryDataMap.Add(InventoryIndex, FInventorySlotData(ItemToAdd, AddibleCount));
		OnInventorySlotUpdated.Broadcast(this, InventoryIndex, ItemToAdd, AddibleCount);
		ItemCountToAdd -= AddibleCount;
	}

	return ItemCountToAdd;
}

int32 UGameplayInventory::RemoveItem(UGameplayItem* ItemToRemove, int32 ItemCountToRemove)
{
	if (ItemToRemove == nullptr)
	{
		return ItemCountToRemove;
	}

	InventoryDataMap.KeySort([](int32 A, int32 B) { return A < B; });

	// Add Item to existing item in inventory
	for (auto Iterator = InventoryDataMap.CreateIterator(); Iterator; ++Iterator)
	{
		if (ItemCountToRemove <= 0)
		{
			break;
		}

		if (Iterator.Value().SlotItem->GetClass() == ItemToRemove->GetClass())
		{
			if (ItemCountToRemove >= Iterator.Value().SlotItemCount)
			{
				int32 RemovableCount = Iterator.Value().SlotItemCount;
				InventoryDataMap[Iterator.Key()].SlotItemCount -= RemovableCount;
				InventoryDataMap[Iterator.Key()].SlotItem = nullptr;
				OnInventorySlotUpdated.Broadcast(this, Iterator.Key(), nullptr, InventoryDataMap[Iterator.Key()].SlotItemCount);
				InventoryDataMap.Remove(Iterator.Key());
				ItemCountToRemove -= RemovableCount;
			}
			else
			{
				InventoryDataMap[Iterator.Key()].SlotItemCount -= ItemCountToRemove;
				OnInventorySlotUpdated.Broadcast(this, Iterator.Key(), ItemToRemove, InventoryDataMap[Iterator.Key()].SlotItemCount);
				ItemCountToRemove = 0;
			}
		}
	}

	return ItemCountToRemove;
}

int32 UGameplayInventory::RemoveItemAtIndex(int32 InventoryIndex, int32 ItemCountToRemove)
{
	FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData == nullptr || ItemCountToRemove <= 0)
	{
		return ItemCountToRemove;
	}

	if (ItemCountToRemove >= InventorySlotData->SlotItemCount)
	{
		int32 RemovableCount = InventorySlotData->SlotItemCount;
		InventorySlotData->SlotItemCount -= RemovableCount;
		InventorySlotData->SlotItem = nullptr;
		OnInventorySlotUpdated.Broadcast(this, InventoryIndex, nullptr, InventorySlotData->SlotItemCount);
		InventoryDataMap.Remove(InventoryIndex);
		ItemCountToRemove -= RemovableCount;
	}
	else
	{
		InventorySlotData->SlotItemCount -= ItemCountToRemove;
		OnInventorySlotUpdated.Broadcast(this, InventoryIndex, InventorySlotData->SlotItem, InventorySlotData->SlotItemCount);
		ItemCountToRemove = 0;
	}

	return ItemCountToRemove;
}

void UGameplayInventory::SwapItemPosition(int32 InventoryIndexA, int32 InventoryIndexB)
{
	FInventorySlotData* InventorySlotDataA = InventoryDataMap.Find(InventoryIndexA);
	FInventorySlotData* InventorySlotDataB = InventoryDataMap.Find(InventoryIndexB);

	if (InventorySlotDataA)
	{
		if (InventorySlotDataB)
		{
			FInventorySlotData TemporaryInventorySlotDataA = InventoryDataMap[InventoryIndexA];
			InventoryDataMap[InventoryIndexA] = InventoryDataMap[InventoryIndexB];
			InventoryDataMap[InventoryIndexB] = TemporaryInventorySlotDataA;
			OnInventorySlotUpdated.Broadcast(this, InventoryIndexA, InventoryDataMap[InventoryIndexA].SlotItem, InventoryDataMap[InventoryIndexA].SlotItemCount);
			OnInventorySlotUpdated.Broadcast(this, InventoryIndexB, InventoryDataMap[InventoryIndexB].SlotItem, InventoryDataMap[InventoryIndexB].SlotItemCount);
		}
		else
		{
			InventoryDataMap.Add(InventoryIndexB, *InventorySlotDataA);
			InventoryDataMap.Remove(InventoryIndexA);
			OnInventorySlotUpdated.Broadcast(this, InventoryIndexA, nullptr, 0);
			OnInventorySlotUpdated.Broadcast(this, InventoryIndexB, InventorySlotDataA->SlotItem, InventorySlotDataA->SlotItemCount);
		}
	}
	else
	{
		if (InventorySlotDataB)
		{
			InventoryDataMap.Add(InventoryIndexA, *InventorySlotDataB);
			InventoryDataMap.Remove(InventoryIndexB);
			OnInventorySlotUpdated.Broadcast(this, InventoryIndexA, InventorySlotDataB->SlotItem, InventorySlotDataB->SlotItemCount);
			OnInventorySlotUpdated.Broadcast(this, InventoryIndexB, nullptr, 0);
		}
	}
}

int32 UGameplayInventory::FillItemToEmptySlot(UGameplayItem* ItemToAdd, int32 ItemCountToAdd)
{
	if (ItemToAdd == nullptr)
	{
		return ItemCountToAdd;
	}

	int32 MaxInventoryIndex = GetMaxIndex();
	for (auto InventoryIndex = 0; InventoryIndex <= MaxInventoryIndex; ++InventoryIndex)
	{
		if (ItemCountToAdd <= 0)
		{
			break;
		}

		if (InventoryDataMap.Find(InventoryIndex) == nullptr)
		{
			int32 MaxAddibleCount = ItemToAdd->GetMaxStackCount() == 0 ? INT_MAX : ItemToAdd->GetMaxStackCount();
			if (ItemCountToAdd > MaxAddibleCount)
			{
				FInventorySlotData InventorySlotDataToAdd;
				InventorySlotDataToAdd.SlotItem = ItemToAdd;
				InventorySlotDataToAdd.SlotItemCount = MaxAddibleCount;
				InventoryDataMap.Add(InventoryIndex, InventorySlotDataToAdd);
				OnInventorySlotUpdated.Broadcast(this, InventoryIndex, ItemToAdd, MaxAddibleCount);
				ItemCountToAdd -= MaxAddibleCount;
			}
			else
			{
				FInventorySlotData InventorySlotDataToAdd;
				InventorySlotDataToAdd.SlotItem = ItemToAdd;
				InventorySlotDataToAdd.SlotItemCount = ItemCountToAdd;
				InventoryDataMap.Add(InventoryIndex, InventorySlotDataToAdd);
				OnInventorySlotUpdated.Broadcast(this, InventoryIndex, ItemToAdd, ItemCountToAdd);
				ItemCountToAdd = 0;
			}
		}
	}

	return ItemCountToAdd;
}

bool UGameplayInventory::UseItem(int32 InventoryIndex)
{
	if (InventorySystem == nullptr)
	{
		return false;
	}

	FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData && InventorySlotData->SlotItem && InventorySlotData->SlotItem->IsUsable() && InventorySlotData->SlotItemCount > 0)
	{
		AController* Controller = Cast<AController>(InventorySystem->GetOwner());
		if (Controller == nullptr)
		{
			return false;
		}

		const APawn* Pawn = Controller->GetPawn();
		if (Pawn == nullptr)
		{
			return false;
		}

		UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);
		if (OwnerAbilitySystemComponent == nullptr)
		{
			return false;
		}

		TSubclassOf<UGameplayAbility> GrantedAbility = InventorySlotData->SlotItem->GetGrantedAbility();
		if (!GrantedAbility)
		{
			return false;
		}

		FGameplayAbilitySpec ItemAbilitySpec = FGameplayAbilitySpec(GrantedAbility, UGameplayEffect::INVALID_LEVEL);
		FGameplayAbilitySpecHandle GameplayAbilitySpecHandle = OwnerAbilitySystemComponent->GiveAbilityAndActivateOnce(ItemAbilitySpec);
		if (GameplayAbilitySpecHandle.IsValid() && InventorySlotData->SlotItem->IsRemovedAfterUse())
		{
			RemoveItemAtIndex(InventoryIndex, 1);
		}

		return true;
	}

	return false;
}

void UGameplayInventory::CombineItem(int32 InventoryIndex)
{
	FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData)
	{
		UGameplayItem* GameplayItem = InventorySlotData->SlotItem;
		int32 GameplayItemCount = InventorySlotData->SlotItemCount;
		if (GameplayItem && GameplayItem->IsStackable() && GameplayItemCount > 0)
		{
			int32 CurrentStackCount = GetItemCountAtIndex(InventoryIndex);
			int32 MaxStackCount = GameplayItem->GetMaxStackCount() == 0 ? INT_MAX : GameplayItem->GetMaxStackCount();

			InventoryDataMap.KeySort([](int32 A, int32 B) { return A < B; });
			for (auto Iterator = InventoryDataMap.CreateIterator(); Iterator; ++Iterator)
			{
				if (CurrentStackCount >= MaxStackCount)
				{
					return;
				}

				if (Iterator.Key() == InventoryIndex || Iterator.Value().SlotItem->GetClass() != GameplayItem->GetClass())
				{
					continue;
				}

				int32 LeftStackCount = MaxStackCount - CurrentStackCount;
				int32 RemoveItemCount = FMath::Clamp(GetItemCountAtIndex(Iterator.Key()), 0, LeftStackCount);
				int32 LeftItemCountAfterRemove = RemoveItemAtIndex(Iterator.Key(), RemoveItemCount);
				int32 LeftItemCountAfterAdd = AddItemAtIndex(InventoryIndex, GameplayItem, RemoveItemCount - LeftItemCountAfterRemove);

				CurrentStackCount += RemoveItemCount - LeftItemCountAfterAdd;
			}
		}
	}
}

void UGameplayInventory::SplitItemInHalf(int32 InventoryIndex)
{
	FInventorySlotData* InventorySlotData = InventoryDataMap.Find(InventoryIndex);
	if (InventorySlotData)
	{
		UGameplayItem* GameplayItem = InventorySlotData->SlotItem;
		int32 GameplayItemCount = InventorySlotData->SlotItemCount;
		if (GameplayItem && GameplayItem->IsStackable() && GameplayItemCount > 1)
		{
			int32 SplitItemCount = GameplayItemCount / 2;
			int32 LeftItemCount = FillItemToEmptySlot(GameplayItem, SplitItemCount);
			RemoveItemAtIndex(InventoryIndex, SplitItemCount - LeftItemCount);
		}
	}
}

UGameplayInventory* UGameplayInventory::CreateInventory(UInventorySystemComponent* OuterInventorySystem, int32 InventoryRow, int32 InventoryColumn, EAccessPermission InventoryAccessPermission)
{
	if (OuterInventorySystem)
	{
		UGameplayInventory* NewInventory = NewObject<UGameplayInventory>(OuterInventorySystem->GetOwner(), UGameplayInventory::StaticClass());
		NewInventory->MaxRow = InventoryRow;
		NewInventory->MaxColumn = InventoryColumn;
		NewInventory->InventorySystem = OuterInventorySystem;
		NewInventory->ExternalAccessPermission = InventoryAccessPermission;

		return NewInventory;
	}

	return nullptr;
}
