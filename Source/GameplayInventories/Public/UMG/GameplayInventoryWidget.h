
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYINVENTORIES_API UGameplayInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

protected:
	UFUNCTION()
	void OnInventorySlotUpdated(class UGameplayInventory* Inventory, int32 InventoryIndex, class UGameplayItem* Item, int32 ItemCount);

protected:
	UPROPERTY(Meta = (BindWidget))
	class UBorder* InventoryBorder;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<class UGameplayInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY()
	TArray<UGameplayInventorySlotWidget*> InventorySlots;

	TWeakObjectPtr<class UInventorySystemComponent> OwningInventorySystemComponent;
	TWeakObjectPtr<class UGameplayInventory> OwningInventory;
};
