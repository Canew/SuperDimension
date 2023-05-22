


#include "UMG/GameplayInventorySlotWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UMG/InventorySlotDragDropOperation.h"
#include "GameplayInventory.h"
#include "InventorySystemComponent.h"

void UGameplayInventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InventorySlotBorder->SetContentColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	InventorySlotBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.4f));
}

void UGameplayInventorySlotWidget::InitializeInventorySlot(TWeakObjectPtr<class UGameplayInventory> NewOwningInventory, int32 NewInventoryIndex)
{
	OwningInventory = NewOwningInventory;
	OwningInventoryIndex = NewInventoryIndex;

	if (!OwningInventory.IsValid() || !OwningInventory->IsValidIndex(OwningInventoryIndex))
	{
		return;
	}

	SlotItem = OwningInventory->GetItemAtIndex(OwningInventoryIndex);
	if (SlotItem)
	{

		InventorySlotImage->SetBrush(SlotItem->GetItemIcon());
		InventorySlotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		FText InventorySlotCountText = FText::FromString(FString::FromInt(OwningInventory->GetItemCountAtIndex(OwningInventoryIndex)));
		InventorySlotCount->SetText(InventorySlotCountText);
		if (SlotItem->IsStackable())
		{
			InventorySlotCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			InventorySlotCount->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		InventorySlotImage->SetVisibility(ESlateVisibility::Collapsed);
		InventorySlotCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameplayInventorySlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

	InventorySlotBorder->SetContentColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	InventorySlotBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.4f));
	UWidgetBlueprintLibrary::CancelDragDrop();
}

void UGameplayInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	InventorySlotBorder->SetContentColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
	InventorySlotBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));
}

void UGameplayInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	InventorySlotBorder->SetContentColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	InventorySlotBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.4f));
}

FReply UGameplayInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	InventorySlotBorder->SetContentColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f));
	InventorySlotBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));

	if (OwningInventory.IsValid() && OwningInventory->GetItemAtIndex(OwningInventoryIndex) && OwningInventory->GetItemCountAtIndex(OwningInventoryIndex) > 0)
	{
		if (InMouseEvent.GetEffectingButton().GetFName() == FName(TEXT("LeftMouseButton")))
		{
			FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, InMouseEvent.GetEffectingButton());
			return EventReply.NativeReply;
		}
		else if (InMouseEvent.GetEffectingButton().GetFName() == FName(TEXT("RightMouseButton")))
		{
			OwningInventory->UseItem(OwningInventoryIndex);
		}
		else if (InMouseEvent.GetEffectingButton().GetFName() == FName(TEXT("MiddleMouseButton")))
		{
			OwningInventory->SplitItemInHalf(OwningInventoryIndex);
		}
	}

	return FReply::Handled();
}

FReply UGameplayInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	if (OwningInventory.IsValid() && OwningInventory->GetItemAtIndex(OwningInventoryIndex) && OwningInventory->GetItemCountAtIndex(OwningInventoryIndex) > 0)
	{
		if (InMouseEvent.GetEffectingButton().GetFName() == FName(TEXT("LeftMouseButton")))
		{
			OwningInventory->CombineItem(OwningInventoryIndex);

			FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, InMouseEvent.GetEffectingButton());
			return EventReply.NativeReply;
		}
		else if (InMouseEvent.GetEffectingButton().GetFName() == FName(TEXT("RightMouseButton")))
		{
			OwningInventory->UseItem(OwningInventoryIndex);
		}
		else if (InMouseEvent.GetEffectingButton().GetFName() == FName(TEXT("MiddleMouseButton")))
		{
			OwningInventory->SplitItemInHalf(OwningInventoryIndex);
		}
	}

	return FReply::Handled();
}

FReply UGameplayInventorySlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	InventorySlotBorder->SetContentColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
	InventorySlotBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));

	return FReply::Handled();
}

void UGameplayInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UInventorySystemComponent* InventorySystemComponent = UInventorySystemComponent::GetInventorySystemComponentFromActor(GetOwningPlayer());
	if (!InventorySystemComponent || InventorySystemComponent->GetExternalAccessPermission(OwningInventory.Get()) != EAccessPermission::ReadWrite)
	{
		return;
	}

	UGameplayInventorySlotWidget* InventorySlotDragWidget = CreateWidget<UGameplayInventorySlotWidget>(GetOwningPlayer(), InventorySlotDragWidgetClass);
	if (InventorySlotDragWidget)
	{
		InventorySlotDragWidget->InitializeInventorySlot(OwningInventory, OwningInventoryIndex);

		UInventorySlotDragDropOperation* InventorySlotDragDropOperation = NewObject<UInventorySlotDragDropOperation>(GetTransientPackage(), InventorySlotDragDropOperationClass);
		if (InventorySlotDragDropOperation)
		{
			InventorySlotDragDropOperation->Pivot = EDragPivot::CenterCenter;
			InventorySlotDragDropOperation->DefaultDragVisual = InventorySlotDragWidget;
			InventorySlotDragDropOperation->DragInventory = OwningInventory;
			InventorySlotDragDropOperation->DragInventoryIndex = OwningInventoryIndex;

			OutOperation = InventorySlotDragDropOperation;
		}
	}
}

bool UGameplayInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventorySystemComponent* InventorySystemComponent = UInventorySystemComponent::GetInventorySystemComponentFromActor(GetOwningPlayer());
	if (!InventorySystemComponent || InventorySystemComponent->GetExternalAccessPermission(OwningInventory.Get()) != EAccessPermission::ReadWrite)
	{
		return true;
	}

	UInventorySlotDragDropOperation* InDragDropOperation = Cast<UInventorySlotDragDropOperation>(InOperation);
	if (!InDragDropOperation)
	{
		return true;
	}

	UGameplayItem* CurrentItem = OwningInventory->GetItemAtIndex(OwningInventoryIndex);
	int32 CurrentItemCount = OwningInventory->GetItemCountAtIndex(OwningInventoryIndex);
	UGameplayItem* DropItem = InventorySystemComponent->GetItemAtIndex(InDragDropOperation->DragInventory.Get(), InDragDropOperation->DragInventoryIndex);
	int32 DropItemCount = InventorySystemComponent->GetItemCountAtIndex(InDragDropOperation->DragInventory.Get(), InDragDropOperation->DragInventoryIndex);
	if (!DropItem || DropItemCount == 0)
	{
		return true;
	}

	if (OwningInventory == InDragDropOperation->DragInventory)
	{
		if (OwningInventoryIndex != InDragDropOperation->DragInventoryIndex)
		{
			if (!CurrentItem || CurrentItemCount == 0 || (CurrentItem->GetClass() == DropItem->GetClass() && CurrentItem->IsStackable() && DropItem->IsStackable()))
			{
				int32 LeftItemCount = OwningInventory->AddItemAtIndex(OwningInventoryIndex, DropItem, DropItemCount);
				int32 RemoveItemCount = DropItemCount - LeftItemCount;
				InDragDropOperation->DragInventory->RemoveItemAtIndex(InDragDropOperation->DragInventoryIndex, RemoveItemCount);
			}
			else
			{
				OwningInventory->SwapItemPosition(OwningInventoryIndex, InDragDropOperation->DragInventoryIndex);
			}
		}
	}
	else
	{
		if (!CurrentItem || CurrentItemCount == 0 || (CurrentItem->GetClass() == DropItem->GetClass() && CurrentItem->IsStackable() && DropItem->IsStackable()))
		{
			int32 LeftItemCount = OwningInventory->AddItemAtIndex(OwningInventoryIndex, DropItem, DropItemCount);
			int32 RemoveItemCount = DropItemCount - LeftItemCount;
			InDragDropOperation->DragInventory->RemoveItemAtIndex(InDragDropOperation->DragInventoryIndex, RemoveItemCount);
		}
		else
		{
			InventorySystemComponent->SwapItemPosition(OwningInventory.Get(), OwningInventoryIndex, InDragDropOperation->DragInventory.Get(), InDragDropOperation->DragInventoryIndex);
		}
	}

	return true;
}
