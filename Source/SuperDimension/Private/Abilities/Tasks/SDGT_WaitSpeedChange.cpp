


#include "Abilities/Tasks/SDGT_WaitSpeedChange.h"
#include "GameFramework/MovementComponent.h"

USDGT_WaitSpeedChange::USDGT_WaitSpeedChange()
{
	bTickingTask = true;
}

void USDGT_WaitSpeedChange::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();
}

void USDGT_WaitSpeedChange::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		EndTask();
		return;
	}

	UMovementComponent* MovementComponent = AvatarActor->FindComponentByClass<UMovementComponent>();
	if (!MovementComponent)
	{
		EndTask();
		return;
	}

	float Speed = MovementComponent->Velocity.Size();
	if (Speed < MinSpeed)
	{
		OnChange.Broadcast();
		EndTask();
	}

}

USDGT_WaitSpeedChange* USDGT_WaitSpeedChange::CreateWaitSpeedChangeEvent(UGameplayAbility* OwningAbility, float MinSpeed)
{
	USDGT_WaitSpeedChange* WaitSpeedChangeTask = NewAbilityTask<USDGT_WaitSpeedChange>(OwningAbility);
	WaitSpeedChangeTask->MinSpeed = MinSpeed;
	return WaitSpeedChangeTask;
}
