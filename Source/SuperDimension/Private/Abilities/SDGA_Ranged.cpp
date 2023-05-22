


#include "Abilities/SDGA_Ranged.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Weapons/SDProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void USDGA_Ranged::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;
	
	UAbilityTask_PlayMontageAndWait* PlayRangedMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, RangedMontage, 1.0f, NAME_None, false);
	if (PlayRangedMontageAndWait)
	{
		PlayRangedMontageAndWait->OnCompleted.AddDynamic(this, &USDGA_Ranged::HandleMontageCompleted);
		PlayRangedMontageAndWait->OnBlendOut.AddDynamic(this, &USDGA_Ranged::HandleMontageBlendOut);
		PlayRangedMontageAndWait->OnInterrupted.AddDynamic(this, &USDGA_Ranged::HandleMontageInterrupted);
		PlayRangedMontageAndWait->OnCancelled.AddDynamic(this, &USDGA_Ranged::HandleMontageCancelled);
		PlayRangedMontageAndWait->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLaunchEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.Montage.Launch")));
		if (WaitLaunchEvent)
		{
			WaitLaunchEvent->EventReceived.AddDynamic(this, &USDGA_Ranged::HandleLaunchEventReceived);
			WaitLaunchEvent->ReadyForActivation();
		}
	}
}

void USDGA_Ranged::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}

void USDGA_Ranged::HandleMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Ranged::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Ranged::HandleMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USDGA_Ranged::HandleMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void USDGA_Ranged::HandleLaunchEventReceived(FGameplayEventData Payload)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !ProjectileClass)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	FVector MuzzleSocketLocation;
	USkeletalMeshComponent* AvatarMesh = AvatarActor->FindComponentByClass<USkeletalMeshComponent>();
	if (AvatarMesh->DoesSocketExist(MuzzleSocketName))
	{
		MuzzleSocketLocation = AvatarMesh->GetSocketLocation(MuzzleSocketName);
	}
	else
	{
		MuzzleSocketLocation = AvatarActor->GetActorLocation();
	}

	ASDProjectile* Projectile = World->SpawnActorDeferred<ASDProjectile>(ProjectileClass, FTransform(FRotator::ZeroRotator, MuzzleSocketLocation), AvatarActor);
	if (Projectile)
	{
		Projectile->GetProjectileMovement()->Velocity = GetProjectileDirection();
		Projectile->FinishSpawning(FTransform(FRotator::ZeroRotator, MuzzleSocketLocation));
	}
}

FVector USDGA_Ranged::GetProjectileDirection() const
{
	FVector ProjectileDirection = FVector(1.0f, 0.0f, 0.0f);

	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!AvatarPawn)
	{
		return ProjectileDirection;
	}

	if (!AvatarPawn->IsPlayerControlled())
	{
		if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
		{
			if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
			{
				if (AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor"))))
				{
					ProjectileDirection = (TargetActor->GetActorLocation() - AvatarPawn->GetActorLocation()).GetSafeNormal();
				}
				else
				{
					ProjectileDirection = AvatarPawn->GetActorForwardVector();
				}
			}
		}
	}

	return ProjectileDirection;
}
