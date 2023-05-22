

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SDInteractionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentInteractionActorChanged, TWeakObjectPtr<AActor>);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPERDIMENSION_API USDInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USDInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual TWeakObjectPtr<AActor> GetCurrentInteractionActor() const;
	virtual class UAnimMontage* GetInteractMontage() const;
	virtual bool CanInteract() const;
	virtual void Interact();

protected:
	virtual TWeakObjectPtr<AActor> FindInteractableActor();
	virtual void ClearInteractableActor();

public:
	FOnCurrentInteractionActorChanged OnCurrentInteractionActorChanged;

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentInteractionActor;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction | Widget")
	TSubclassOf<class UUserWidget> InteractionWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction | Widget")
	FVector InteractionWidgetRelativeLocation;

	UPROPERTY()
	class UWidgetComponent* InteractionWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction | Animation")
	class UAnimMontage* InteractMontage;
};
