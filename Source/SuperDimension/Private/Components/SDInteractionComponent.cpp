


#include "Components/SDInteractionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/SDInteractionInterface.h"

// Sets default values for this component's properties
USDInteractionComponent::USDInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	InteractionRadius = 150.0f;
	InteractionWidgetRelativeLocation = FVector(0.0f, 0.0f, 20.0f);
}


// Called when the game starts
void USDInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	InteractionWidgetComponent = NewObject<UWidgetComponent>(this, FName(TEXT("InteractionWidget")));
	if (InteractionWidgetComponent)
	{
		InteractionWidgetComponent->RegisterComponent();
		InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass);
		InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		InteractionWidgetComponent->SetDrawAtDesiredSize(true);
		InteractionWidgetComponent->InitWidget();
		if (UUserWidget* InteractionWidget = InteractionWidgetComponent->GetUserWidgetObject())
		{
			InteractionWidget->Initialize();
		}

		InteractionWidgetComponent->SetVisibility(false);
		InteractionWidgetComponent->SetRelativeLocation(InteractionWidgetRelativeLocation);
	}
}


// Called every frame
void USDInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ISDInteractionInterface* InteractionInterface = Cast<ISDInteractionInterface>(CurrentInteractionActor))
	{
		if (!CanInteract() || !InteractionInterface->CanInteract() || GetOwner()->GetDistanceTo(CurrentInteractionActor.Get()) > InteractionRadius)
		{
			ClearInteractableActor();
			return;
		}
	}

	if (CanInteract())
	{
		TWeakObjectPtr<AActor> OldInteractionActor = CurrentInteractionActor;
		CurrentInteractionActor = FindInteractableActor();
		ISDInteractionInterface* InteractionInterface = Cast<ISDInteractionInterface>(CurrentInteractionActor);
		if (InteractionInterface && OldInteractionActor != CurrentInteractionActor)
		{
			InteractionInterface->OnInteractionEnabled();
			InteractionWidgetComponent->SetVisibility(true);
			InteractionWidgetComponent->AttachToComponent(CurrentInteractionActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			OnCurrentInteractionActorChanged.Broadcast(CurrentInteractionActor.Get());
		}
		else
		{
			ClearInteractableActor();
		}
	}
}

TWeakObjectPtr<AActor> USDInteractionComponent::GetCurrentInteractionActor() const
{
	return CurrentInteractionActor;
}

UAnimMontage* USDInteractionComponent::GetInteractMontage() const
{
	return InteractMontage;
}

bool USDInteractionComponent::CanInteract() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter && (OwnerCharacter->GetCharacterMovement()->IsFalling() || OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()))
	{
		return false;
	}

	return true;
}

void USDInteractionComponent::Interact()
{
	if (!CanInteract())
	{
		return;
	}

	if (ISDInteractionInterface* InteractionInterface = Cast<ISDInteractionInterface>(CurrentInteractionActor))
	{
		InteractionInterface->Interact();
	}
}

TWeakObjectPtr<AActor> USDInteractionComponent::FindInteractableActor()
{
	UWorld* World = GetOwner()->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TWeakObjectPtr<AActor> InteractionActor;
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByProfile(OverlapResults, GetOwner()->GetActorLocation(), FQuat::Identity, TEXT("OverlapAll"), FCollisionShape::MakeSphere(InteractionRadius));
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* OverlapActor = OverlapResult.GetActor();
		if (!OverlapActor || GetOwner()->GetDistanceTo(OverlapActor) > InteractionRadius)
		{
			continue;
		}

		if (ISDInteractionInterface* OverlapInteractionInterface = Cast<ISDInteractionInterface>(OverlapActor))
		{
			if (!OverlapInteractionInterface->CanInteract())
			{
				continue;
			}

			if (!CurrentInteractionActor.IsValid() || GetOwner()->GetDistanceTo(OverlapActor) < GetOwner()->GetDistanceTo(CurrentInteractionActor.Get()))
			{
				InteractionActor = OverlapActor;
			}
		}
	}

	return InteractionActor;
}

void USDInteractionComponent::ClearInteractableActor()
{
	if (ISDInteractionInterface* InteractionInterface = Cast<ISDInteractionInterface>(CurrentInteractionActor))
	{
		InteractionInterface->OnInteractionDisabled();
		InteractionWidgetComponent->SetVisibility(false);
		InteractionWidgetComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		OnCurrentInteractionActorChanged.Broadcast(nullptr);
	}

	CurrentInteractionActor = nullptr;
}
