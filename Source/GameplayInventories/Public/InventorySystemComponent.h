// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayInventory.h"
#include "GameplayItem.h"
#include "InventorySystemComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAYINVENTORIES_API UInventorySystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventorySystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	/** Called when level startup or actor spawn. This is before BeginPlay */
	virtual void InitializeComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// manually register inventory to this inventory system component
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RegisterInventory(class UGameplayInventory* TargetInventory);

	// Return jnventory's external access permission. if system owns inventory always return EAccessPermission:ReadWrite
	UFUNCTION(BlueprintCallable, Category = Inventory)
	EAccessPermission GetExternalAccessPermission(class UGameplayInventory* TargetInventory) const;

	/** Get total item count in entire inventories */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UPARAM(DisplayName = "Item Count") int32 GetItemCount(TSubclassOf<class UGameplayItem> ItemClass) const;

	/** Get total item count in inventory */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UPARAM(DisplayName = "Item Count") int32 GetItemCountAtInventory(UGameplayInventory* TargetInventory, TSubclassOf<class UGameplayItem> ItemClass) const;
	
	/** Add item to specific index */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UPARAM(DisplayName = "Item") class UGameplayItem* GetItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex) const;

	/** Add item to specific index */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UPARAM(DisplayName = "Item Count") int32 GetItemCountAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex) const;

	/** Add item to addible slot across whole inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 AddItem(UGameplayItem* ItemToCreate, int32 ItemCountToCreate);

	/** Add item to addible slot by class across whole inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 AddItemByClass(TSubclassOf<UGameplayItem> ItemClassToCreate, int32 ItemCountToCreate);

	/** Add item to addible slot in specific inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 AddItemAtInventory(UGameplayInventory* TargetInventory, UGameplayItem* ItemToAdd, int32 ItemCountToAdd);

	/** Add item to specified index in specific inventory index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 AddItemAtIndex(UGameplayInventory* TargetInventory,int32 InventoryIndex, UGameplayItem* ItemToAdd, int32 ItemCountToAdd);

	/** Remove item from across whole inventory 
	  * @note regardless of remaining item count (Remove even though ItemCountToRemove > LeftItemCount)
	  */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 RemoveItem(UGameplayItem* ItemToRemove, int32 ItemCountToRemove);

	/** Remove item from specific inventory
	  * @note regardless of remaining item count (Remove even though ItemCountToRemove > LeftItemCount)
	  */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 RemoveItemAtInventory(UGameplayInventory* TargetInventory, UGameplayItem* ItemToRemove, int32 ItemCountToRemove);

	/** Remove item from specified index in inventory
	  * @note regardless of remaining item count (Remove even though ItemCountToRemove > LeftItemCount)
	  */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UPARAM(DisplayName = "Left Item Count") int32 RemoveItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex, int32 ItemCountToRemove);

	/** Swap item position between slot a in inventory a and slot b in inventory b */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SwapItemPosition(UGameplayInventory* TargetInventoryA, int32 InventoryIndexA, UGameplayInventory* TargetInventoryB, int32 InventoryIndexB);

	/** Use item in specific index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool UseItemAtIndex(UGameplayInventory* TargetInventory, int32 InventoryIndex);

	/** Searches the passed in actor for an inventory system component, will use the InventorySystemInterface */
	UFUNCTION(BlueprintPure, Category = Inventory)
	static UInventorySystemComponent* GetInventorySystemComponentFromActor(AActor* Actor, bool LookForComponent = false);

	/** Return inventories registered to this inventory system */
	UFUNCTION(BlueprintPure, Category = Inventory, DisplayName = "Get Inventories")
	TArray<UGameplayInventory*> GetSpawnedInventories() const;

private:
	UPROPERTY(EditAnywhere, Category = "Initial Inventory")
	TArray<FInventoryDefaults> InitialInventoryData;

	/** Currently registered inventories */
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TArray<UGameplayInventory*> SpawnedInventories;

};
