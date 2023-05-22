

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SDInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class USDInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SUPERDIMENSION_API ISDInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual FText GetInteractionDescription() const { return FText::FromString(TEXT("")); }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual bool CanInteract() const { return true; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact() = 0;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void OnInteractionEnabled() {}
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void OnInteractionDisabled() {}
};
