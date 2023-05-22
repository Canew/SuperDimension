

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventorySlotDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYINVENTORIES_API UInventorySlotDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	TWeakObjectPtr<class UGameplayInventory> DragInventory;
	int32 DragInventoryIndex;
};
