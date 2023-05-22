

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SDInteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDInteractWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized();

public:
	virtual void SetInteractKeyText(FText InteractKeyText);
	virtual void SetInteractDescriptionText(FText InteractDescriptionText);

protected:
	virtual void HandleCurrentInteractionActorChanged(TWeakObjectPtr<AActor> InteractionActor);
	
protected:
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* InteractKeyTextBlock;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* InteractDescriptionTextBlock;

	TWeakObjectPtr<class USDInteractionComponent> InteractionComponent;
};
