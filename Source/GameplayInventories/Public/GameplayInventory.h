#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayItem.h"
#include "GameplayInventory.generated.h"

/** Notification delegate definition for when inventory slot updated in this inventory */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInventorySlotUpdatedSignature, UGameplayInventory*, Inventory, int32, InventoryIndex, UGameplayItem*, Item, int32, ItemCount);

/** Degree of inventory disclosure to external access */
UENUM(BlueprintType)
enum class EAccessPermission : uint8
{
	Denied,			// inventory is not exposed to external this inventory
	ReadOnly,		// only readable from external this inventory
	ReadWrite		// can read and write from external this inventory
};

/** Structure that represents one inventory slot in inventory */
USTRUCT(BlueprintType)
struct FInventorySlotData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
	UGameplayItem* SlotItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
	int32 SlotItemCount;

	FInventorySlotData()
		: SlotItem(nullptr), SlotItemCount(0) {}

	FInventorySlotData(UGameplayItem* Item, int32 ItemCount)
		: SlotItem(Item), SlotItemCount(ItemCount) {}

	FInventorySlotData(UGameplayItem* Item)
		: SlotItem(Item), SlotItemCount(0) {}

	bool operator==(FInventorySlotData OtherSlotData)
	{
		return SlotItem == OtherSlotData.SlotItem;
	}
};

/** Structure that defines initial inventory configuration */
USTRUCT(BlueprintType)
struct FInventoryDefaults
{
	GENERATED_USTRUCT_BODY()

public:
	FInventoryDefaults()
		: DefaultAccessPermission(EAccessPermission::Denied) {}

	UPROPERTY(EditAnywhere, Category = "Inventory Default")
	int32 MaxRow;

	UPROPERTY(EditAnywhere, Category = "Inventory Default")
	int32 MaxColumn;

	UPROPERTY(EditAnywhere, Category = "Inventory Default")
	EAccessPermission DefaultAccessPermission;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYINVENTORIES_API UGameplayInventory : public UObject
{
	GENERATED_BODY()

public:
	/** Get total item count in this inventory */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Item Count") int32 GetItemCount(TSubclassOf<class UGameplayItem> ItemClass) const;

	/** Get item count in specific index */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Item") class UGameplayItem* GetItemAtIndex(int32 InventoryIndex) const;

	/** Get item count in specific index */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Item Count") int32 GetItemCountAtIndex(int32 InventoryIndex) const;

	/** Add item to addible slot by class
	  * @return Return number of items not added
	  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Left Item Count") int32 AddItemByClass(TSubclassOf<UGameplayItem> ItemToCreate, int32 ItemCountToCreate,
		bool bFillLeftItemToEmptySlot = true);

	/** Add item to addible slot from first slot
	  * @return Return number of items not added
	  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Left Item Count") int32 AddItem(UGameplayItem* ItemToAdd, int32 ItemCountToAdd,
		bool bFillLeftItemToEmptySlot = true);

	/** Add item to specific index
	  * @return Return number of items not added
	  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Left Item Count") int32 AddItemAtIndex(int32 InventoryIndex, UGameplayItem* ItemToAdd, int32 ItemCountToAdd);

protected:
	/** Fill item to empty slot from the front
	  * @return Return number of items not added
	  */
	UPARAM(DisplayName = "Left Item Count") int32 FillItemToEmptySlot(UGameplayItem* ItemToAdd, int32 ItemCountToAdd);

public:
	/** Remove item as many as ItemCountToRemove from inventory
	  * @return Return number of items not removed 
	  * @note Always remove regardless of remaining item count (Remove even though ItemCountToRemove > LeftItemCount)
	  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Left Item Count") int32 RemoveItem(UGameplayItem* ItemToRemove, int32 ItemCountToRemove);

	/** Remove item as many as ItemCountToRemove from specific index in inventory
	  * @return Return number of items not removed
	  * @note Always remove regardless of remaining item count (Remove even though ItemCountToRemove > LeftItemCount)
	  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UPARAM(DisplayName = "Left Item Count") int32 RemoveItemAtIndex(int32 InventoryIndex, int32 ItemCountToRemove);

	/** Swap item position between slot a and slot b */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwapItemPosition(int32 InventoryIndexA, int32 InventoryIndexB);

	/** Use item in specific slot, return true if success to use */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(int32 InventoryIndex);

	/** Combine item of same kind as an item at a specifin index specific index in a inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CombineItem(int32 InventoryIndex);

	/** Split item in half in specific inventory slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SplitItemInHalf(int32 InventoryIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Is Valid") FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index <= GetMaxIndex();
	}

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Max Row") FORCEINLINE int32 GetMaxRow() const
	{
		return MaxRow;
	}
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Max Column") FORCEINLINE int32 GetMaxColumn() const
	{
		return MaxColumn;
	}

	/** Return inventory index from row/column (Row * MaxColumn + Column), return -1 if out of bound */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE int32 GetInventoryIndexFromRowAndColumn(int32 Row, int32 Column) const
	{
		if (Row < 0 || Row >= MaxRow || Column < 0 || Column >= MaxColumn)
		{
			return -1;
		}

		return Row * MaxColumn + Column;
	}

	/** Return inventory row and column from index, return -1 if out of bound */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPARAM(DisplayName = "Is Valid Index") FORCEINLINE bool GetRowAndColumnFromIndex(int32 Index, int32& Row, int32& Column) const
	{
		if (Index < 0 || Index > GetMaxIndex())
		{
			Row = -1;
			Column = -1;
			return false;
		}

		Row = Index / MaxColumn;
		Column = Index % MaxColumn;
		return true;
	}

	FORCEINLINE int32 GetMaxIndex() const
	{
		return (MaxRow * MaxColumn) - 1;
	}

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE EAccessPermission GetExternalAccessPermission() const { return ExternalAccessPermission; }

	/** static helper function for create new inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static class UGameplayInventory* CreateInventory(class UInventorySystemComponent* InventorySystem, int32 InventoryRow, int32 InventoryColumn, EAccessPermission InventoryAccessPermission = EAccessPermission::Denied);

public:
	// Called when inventory slot content changed
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventorySlotUpdatedSignature OnInventorySlotUpdated;
	
protected:
	/** Inventory map represents actual inventory content (Key: Inventory slot index, Value: Inventory slot data) */
	UPROPERTY()
	TMap<int32, FInventorySlotData> InventoryDataMap;

	/** InventorySystemComponent that is owning this inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), Category = "Inventory")
	class UInventorySystemComponent* InventorySystem;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Inventory)
	int32 MaxRow;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Inventory)
	int32 MaxColumn;

	/** AccessPermission for external access that is not owning this inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), Category = "Inventory")
	EAccessPermission ExternalAccessPermission;
};
