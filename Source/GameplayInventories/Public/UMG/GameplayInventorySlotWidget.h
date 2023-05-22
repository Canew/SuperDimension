

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayInventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYINVENTORIES_API UGameplayInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	virtual void InitializeInventorySlot(TWeakObjectPtr<class UGameplayInventory> NewOwningInventory, int32 NewInventoryIndex);

protected:
	virtual void NativeDestruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

protected:
	UPROPERTY(Meta = (BindWidget))
	class UBorder* InventorySlotBorder;

	UPROPERTY(Meta = (BindWidget))
	class UImage* InventorySlotImage;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* InventorySlotCount;

	TWeakObjectPtr<class UGameplayInventory> OwningInventory;
	int32 OwningInventoryIndex;

	UPROPERTY()
	class UGameplayItem* SlotItem;

	UPROPERTY(EditDefaultsOnly, Category = "Drag")
	TSubclassOf<class UDragDropOperation> InventorySlotDragDropOperationClass;

	UPROPERTY(EditDefaultsOnly, Category = "Drag")
	TSubclassOf<class UGameplayInventorySlotWidget> InventorySlotDragWidgetClass;
};
