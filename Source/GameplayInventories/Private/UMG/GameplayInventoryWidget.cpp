
#include "UMG/GameplayInventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Border.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/WidgetTree.h"
#include "InventorySystemComponent.h"
#include "InventorySystemInterface.h"
#include "UMG/GameplayInventorySlotWidget.h"

void UGameplayInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	IInventorySystemInterface* InventorySystemInterface = Cast<IInventorySystemInterface>(GetOwningPlayer());
	if (InventorySystemInterface)
	{
		OwningInventorySystemComponent = InventorySystemInterface->GetInventorySystemComponent();
	}

	if (!OwningInventorySystemComponent.IsValid() || OwningInventorySystemComponent->GetSpawnedInventories().Num() <= 0)
	{
		return;
	}

	OwningInventory = OwningInventorySystemComponent->GetSpawnedInventories()[0];
	if (OwningInventory.IsValid())
	{
		UUniformGridPanel* InventoryPanel = WidgetTree->ConstructWidget<UUniformGridPanel>();
		if (InventoryPanel)
		{
			for (int SlotIndex = 0; SlotIndex <= OwningInventory->GetMaxIndex(); SlotIndex++)
			{
				UGameplayInventorySlotWidget* InventorySlot = CreateWidget<UGameplayInventorySlotWidget>(GetOwningPlayer(), InventorySlotWidgetClass);
				if (InventorySlot)
				{
					//Inventory->OnInventorySlotUpdated.AddDynamic()
					InventorySlot->InitializeInventorySlot(OwningInventory, SlotIndex);
					InventorySlot->SetPadding(FMargin(2.0f));

					int32 PanelRow, PanelColumn;
					OwningInventory->GetRowAndColumnFromIndex(SlotIndex, PanelRow, PanelColumn);
					UUniformGridSlot* InventoryPanelSlot = InventoryPanel->AddChildToUniformGrid(InventorySlot, PanelRow, PanelColumn);
					if (InventoryPanelSlot)
					{
						InventoryPanelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
						InventoryPanelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
					}

					InventorySlots.Add(InventorySlot);
				}
			}

			InventoryBorder->AddChild(InventoryPanel);
		}

		OwningInventory->OnInventorySlotUpdated.AddDynamic(this, &UGameplayInventoryWidget::OnInventorySlotUpdated);
	}
}

void UGameplayInventoryWidget::OnInventorySlotUpdated(UGameplayInventory* Inventory, int32 InventoryIndex, UGameplayItem* Item, int32 ItemCount)
{
	if (OwningInventory == Inventory && InventorySlots.IsValidIndex(InventoryIndex))
	{
		InventorySlots[InventoryIndex]->InitializeInventorySlot(Inventory, InventoryIndex);
	}
}
