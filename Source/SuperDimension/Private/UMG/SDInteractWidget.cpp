


#include "UMG/SDInteractWidget.h"
#include "Components/TextBlock.h"
#include "Components/SDInteractionComponent.h"
#include "Interfaces/SDInteractionInterface.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "SDPlayerCharacter.h"

void USDInteractWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ASDPlayerCharacter* PlayerCharacter = Cast<ASDPlayerCharacter>(GetOwningPlayerPawn()))
	{
		UInputMappingContext* CharacterInputMappingContext = PlayerCharacter->GetInputMappingContext();
		const TArray<FEnhancedActionKeyMapping>& Mappings = CharacterInputMappingContext->GetMappings();
		for (const FEnhancedActionKeyMapping& Mapping : Mappings)
		{
			if (Mapping.Action == PlayerCharacter->GetInteractInputAction())
			{
				SetInteractKeyText(FText::FromName(Mapping.Key.GetFName()));
				break;
			}
		}

		InteractionComponent = PlayerCharacter->FindComponentByClass<USDInteractionComponent>();
		if (InteractionComponent.IsValid())
		{
			InteractionComponent->OnCurrentInteractionActorChanged.AddUObject(this, &USDInteractWidget::HandleCurrentInteractionActorChanged);
		}
	}
}

void USDInteractWidget::SetInteractKeyText(FText InteractKeyText)
{
	if (InteractKeyTextBlock)
	{
		InteractKeyTextBlock->SetText(InteractKeyText);
	}
}

void USDInteractWidget::SetInteractDescriptionText(FText InteractDescriptionText)
{
	if (InteractDescriptionTextBlock)
	{
		InteractDescriptionTextBlock->SetText(InteractDescriptionText);
	}
}

void USDInteractWidget::HandleCurrentInteractionActorChanged(TWeakObjectPtr<AActor> InteractionActor)
{
	if (ISDInteractionInterface* InteractionInterface = Cast<ISDInteractionInterface>(InteractionActor))
	{
		SetInteractDescriptionText(InteractionInterface->GetInteractionDescription());
	}
}
