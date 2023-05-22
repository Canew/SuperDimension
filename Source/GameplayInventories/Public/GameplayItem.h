
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayItem.generated.h"

/**
 * 
 */
UCLASS(abstract, BlueprintType, Blueprintable)
class GAMEPLAYINVENTORIES_API UGameplayItem : public UObject
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGameplayItem()
		: MaxStackCount(1)
	{}

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE FText GetItemName() const { return ItemName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE FText GetItemDescription() const { return ItemDescription; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE FSlateBrush GetItemIcon() const { return ItemIcon; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE int32 GetMaxStackCount() const { return MaxStackCount; }

	/** Returns if the item is stackable */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE bool IsStackable() const { return MaxStackCount > 1 || MaxStackCount == 0; };

	/** Returns if the item is consumable */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE bool IsUsable() const { return bUsable; }

	/** Returns if the item has to be removed after use */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE bool IsRemovedAfterUse() const { return bRemovedAfterUse; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayInventories | Item")
	FORCEINLINE TSubclassOf<class UGameplayAbility> GetGrantedAbility() { return GrantedAbility; }

protected:
	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;

	/** Max stack count of instances that can be in inventory slot at once, MaxStackCount == 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stack)
	int32 MaxStackCount;

	/** Whether the item is usable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	bool bUsable;

	/** Whether the item has to be removed after use */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (EditCondition = bUsable))
	bool bRemovedAfterUse;

	/** Ability to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<class UGameplayAbility> GrantedAbility;
};
