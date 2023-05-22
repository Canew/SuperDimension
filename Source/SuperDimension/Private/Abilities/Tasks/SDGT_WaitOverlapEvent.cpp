
#include "Abilities/Tasks/SDGT_WaitOverlapEvent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

USDGT_WaitOverlapEvent::USDGT_WaitOverlapEvent()
{
	bTickingTask = true;
	TickInterval = 0.0f;
	AccumulatedDeltaTime = 0.0f;
	LastTickOverlapComponentLocation = FVector::ZeroVector;
}

void USDGT_WaitOverlapEvent::Activate()
{
	SetWaitingOnAvatar();

	UPrimitiveComponent* const AttachComponent = FindAttachComponent();
	if (!AttachComponent)
	{
		EndTask();
		return;
	}

	if (OverlapCollisionShape.IsBox())
	{
		UBoxComponent* BoxComponent = NewObject<UBoxComponent>(AttachComponent);
		BoxComponent->SetBoxExtent(OverlapCollisionShape.GetBox());
		OverlapComponent = BoxComponent;
	}
	else if (OverlapCollisionShape.IsSphere())
	{
		USphereComponent* SphereComponent = NewObject<USphereComponent>(AttachComponent);
		SphereComponent->SetSphereRadius(OverlapCollisionShape.GetSphereRadius());
		OverlapComponent = SphereComponent;
	}
	else if (OverlapCollisionShape.IsCapsule())
	{
		UCapsuleComponent* CapsuleComponent = NewObject<UCapsuleComponent>(AttachComponent);
		CapsuleComponent->SetCapsuleRadius(OverlapCollisionShape.GetCapsuleRadius());
		CapsuleComponent->SetCapsuleHalfHeight(OverlapCollisionShape.GetCapsuleHalfHeight());
		OverlapComponent = CapsuleComponent;
	}

	if (!OverlapComponent)
	{
		EndTask();
		return;
	}

	if (WaitOverlapEventParams.bAttachToAvatarActor)
	{
		OverlapComponent->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, WaitOverlapEventParams.CollisionAttachSocketName);
	}

	const FTransform& AttachComponentTransform =
		AttachComponent->DoesSocketExist(WaitOverlapEventParams.CollisionAttachSocketName)
		? AttachComponent->GetSocketTransform(WaitOverlapEventParams.CollisionAttachSocketName)
		: AttachComponent->GetComponentTransform();
	FTransform OverlapComponentTransform;
	OverlapComponentTransform.SetRotation(AttachComponentTransform.GetRotation() * FQuat(WaitOverlapEventParams.RotationOffset));
	OverlapComponentTransform.SetLocation(AttachComponentTransform.TransformPosition(WaitOverlapEventParams.LocationOffset));
	OverlapComponent->SetWorldTransform(OverlapComponentTransform);
	OverlapComponent->SetGenerateOverlapEvents(false);
	OverlapComponent->RegisterComponent();

	if (WaitOverlapEventParams.bDebug)
	{
		OverlapComponent->SetVisibility(true);
		OverlapComponent->SetHiddenInGame(false);
	}

	AccumulatedDeltaTime = TickInterval;
	LastTickOverlapComponentLocation = GetOverlapComponent()->GetComponentLocation();
}

void USDGT_WaitOverlapEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	AccumulatedDeltaTime += DeltaTime;
	const bool bTick = AccumulatedDeltaTime >= TickInterval;
	if (bTick)
	{
		AccumulatedDeltaTime = 0.0f;

		SweepOverlapComponent();
	}
}

USDGT_WaitOverlapEvent* USDGT_WaitOverlapEvent::CreateWaitOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, FCollisionShape CollisionShape, FWaitOverlapEventParams WaitOverlapEventParams)
{
	USDGT_WaitOverlapEvent* WaitOverlapEventTask = NewAbilityTask<USDGT_WaitOverlapEvent>(OwningAbility, TaskInstanceName);
	WaitOverlapEventTask->OverlapCollisionShape = CollisionShape;
	WaitOverlapEventTask->WaitOverlapEventParams.bAttachToAvatarActor = WaitOverlapEventParams.bAttachToAvatarActor;
	WaitOverlapEventTask->WaitOverlapEventParams.bOverlapOnlyOnce = WaitOverlapEventParams.bOverlapOnlyOnce;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionAttachSocketName = WaitOverlapEventParams.CollisionAttachSocketName;
	WaitOverlapEventTask->WaitOverlapEventParams.LocationOffset = WaitOverlapEventParams.LocationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.RotationOffset = WaitOverlapEventParams.RotationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionProfileName = WaitOverlapEventParams.CollisionProfileName;
	WaitOverlapEventTask->WaitOverlapEventParams.bDebug = WaitOverlapEventParams.bDebug;
	return WaitOverlapEventTask;
}

USDGT_WaitOverlapEvent* USDGT_WaitOverlapEvent::CreateWaitBoxOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, FVector BoxHalfExtent, FWaitOverlapEventParams WaitOverlapEventParams)
{
	USDGT_WaitOverlapEvent* WaitOverlapEventTask = NewAbilityTask<USDGT_WaitOverlapEvent>(OwningAbility, TaskInstanceName);
	WaitOverlapEventTask->OverlapCollisionShape.SetBox(BoxHalfExtent);
	WaitOverlapEventTask->WaitOverlapEventParams.bAttachToAvatarActor = WaitOverlapEventParams.bAttachToAvatarActor;
	WaitOverlapEventTask->WaitOverlapEventParams.bOverlapOnlyOnce = WaitOverlapEventParams.bOverlapOnlyOnce;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionAttachSocketName = WaitOverlapEventParams.CollisionAttachSocketName;
	WaitOverlapEventTask->WaitOverlapEventParams.LocationOffset = WaitOverlapEventParams.LocationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.RotationOffset = WaitOverlapEventParams.RotationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionProfileName = WaitOverlapEventParams.CollisionProfileName;
	WaitOverlapEventTask->WaitOverlapEventParams.bDebug = WaitOverlapEventParams.bDebug;
	return WaitOverlapEventTask;
}

USDGT_WaitOverlapEvent* USDGT_WaitOverlapEvent::CreateWaitSphereOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, float Radius, FWaitOverlapEventParams WaitOverlapEventParams)
{
	USDGT_WaitOverlapEvent* WaitOverlapEventTask = NewAbilityTask<USDGT_WaitOverlapEvent>(OwningAbility, TaskInstanceName);
	WaitOverlapEventTask->OverlapCollisionShape.SetSphere(Radius);
	WaitOverlapEventTask->WaitOverlapEventParams.bAttachToAvatarActor = WaitOverlapEventParams.bAttachToAvatarActor;
	WaitOverlapEventTask->WaitOverlapEventParams.bOverlapOnlyOnce = WaitOverlapEventParams.bOverlapOnlyOnce;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionAttachSocketName = WaitOverlapEventParams.CollisionAttachSocketName;
	WaitOverlapEventTask->WaitOverlapEventParams.LocationOffset = WaitOverlapEventParams.LocationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.RotationOffset = WaitOverlapEventParams.RotationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionProfileName = WaitOverlapEventParams.CollisionProfileName;
	WaitOverlapEventTask->WaitOverlapEventParams.bDebug = WaitOverlapEventParams.bDebug;
	return WaitOverlapEventTask;
}

USDGT_WaitOverlapEvent* USDGT_WaitOverlapEvent::CreateWaitCapsuleOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, float Radius, float HalfHeight, FWaitOverlapEventParams WaitOverlapEventParams)
{
	USDGT_WaitOverlapEvent* WaitOverlapEventTask = NewAbilityTask<USDGT_WaitOverlapEvent>(OwningAbility, TaskInstanceName);
	WaitOverlapEventTask->OverlapCollisionShape.SetCapsule(Radius, HalfHeight);
	WaitOverlapEventTask->WaitOverlapEventParams.bAttachToAvatarActor = WaitOverlapEventParams.bAttachToAvatarActor;
	WaitOverlapEventTask->WaitOverlapEventParams.bOverlapOnlyOnce = WaitOverlapEventParams.bOverlapOnlyOnce;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionAttachSocketName = WaitOverlapEventParams.CollisionAttachSocketName;
	WaitOverlapEventTask->WaitOverlapEventParams.LocationOffset = WaitOverlapEventParams.LocationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.RotationOffset = WaitOverlapEventParams.RotationOffset;
	WaitOverlapEventTask->WaitOverlapEventParams.CollisionProfileName = WaitOverlapEventParams.CollisionProfileName;
	WaitOverlapEventTask->WaitOverlapEventParams.bDebug = WaitOverlapEventParams.bDebug;
	return WaitOverlapEventTask;
}

void USDGT_WaitOverlapEvent::AddIgnoreActor(AActor* IgnoreActor)
{
	if (IgnoreActor && !IgnoreActor->IsPendingKill())
	{
		OverlapIgnoreActors.AddUnique(IgnoreActor);
	}
}

void USDGT_WaitOverlapEvent::AddIgnoreActors(TArray<AActor*> IgnoreActors)
{
	for (AActor* IgnoreActor : IgnoreActors)
	{
		OverlapIgnoreActors.AddUnique(IgnoreActor);
	}
}

UPrimitiveComponent* USDGT_WaitOverlapEvent::GetOverlapComponent() const
{
	return OverlapComponent;
}

void USDGT_WaitOverlapEvent::OnDestroy(bool AbilityEnded)
{
	SweepOverlapComponent();

	Super::OnDestroy(AbilityEnded);

	if (OverlapComponent)
	{
		OverlapComponent->DestroyComponent();
		OverlapComponent = nullptr;
	}
}

UPrimitiveComponent* USDGT_WaitOverlapEvent::FindAttachComponent()
{
	UPrimitiveComponent* PrimComponent = nullptr;
	AActor* TaskAvatarActor = GetAvatarActor();
	if (TaskAvatarActor)
	{
		PrimComponent = TaskAvatarActor->FindComponentByClass<USkeletalMeshComponent>();
		if (!PrimComponent)
		{
			PrimComponent = Cast<UPrimitiveComponent>(TaskAvatarActor->GetRootComponent());
		}
	}

	return PrimComponent;
}

void USDGT_WaitOverlapEvent::SweepOverlapComponent()
{
	if (OverlapComponent && !OverlapComponent->IsPendingKill() && !LastTickOverlapComponentLocation.IsNearlyZero())
	{
		if (UWorld* World = OverlapComponent->GetWorld())
		{
			TArray<FHitResult> HitResults;

			if (WaitOverlapEventParams.bDebug)
			{
				if (OverlapCollisionShape.IsBox())
				{
					UKismetSystemLibrary::BoxTraceMultiByProfile(
						World,
						LastTickOverlapComponentLocation, OverlapComponent->GetComponentLocation(),
						OverlapCollisionShape.GetExtent(), WaitOverlapEventParams.RotationOffset,
						WaitOverlapEventParams.CollisionProfileName, false, OverlapIgnoreActors,
						EDrawDebugTrace::ForDuration, HitResults, true);
				}
				else if (OverlapCollisionShape.IsSphere())
				{
					UKismetSystemLibrary::SphereTraceMultiByProfile(
						World,
						LastTickOverlapComponentLocation, OverlapComponent->GetComponentLocation(),
						OverlapCollisionShape.GetSphereRadius(),
						WaitOverlapEventParams.CollisionProfileName, false, OverlapIgnoreActors,
						EDrawDebugTrace::ForDuration, HitResults, true);
				}
				else if (OverlapCollisionShape.IsCapsule())
				{
					UKismetSystemLibrary::CapsuleTraceMultiByProfile(
						World,
						LastTickOverlapComponentLocation, OverlapComponent->GetComponentLocation(),
						OverlapCollisionShape.GetCapsuleRadius(), OverlapCollisionShape.GetCapsuleHalfHeight(),
						WaitOverlapEventParams.CollisionProfileName, false, OverlapIgnoreActors,
						EDrawDebugTrace::ForDuration, HitResults, true);
				}
			}
			else
			{
				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActors(OverlapIgnoreActors);
				CollisionQueryParams.bReturnPhysicalMaterial = true;
				World->SweepMultiByProfile(
					HitResults,
					LastTickOverlapComponentLocation, OverlapComponent->GetComponentLocation(), WaitOverlapEventParams.RotationOffset.Quaternion(),
					WaitOverlapEventParams.CollisionProfileName, OverlapCollisionShape, CollisionQueryParams);
			}

			for (const FHitResult& HitResult : HitResults)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor && !HitActor->IsPendingKill() && !OverlappedActors.Contains(HitActor))
				{
					OverlappedActors.AddUnique(HitActor);
					OnOverlap.Broadcast(GetOverlapComponent(), HitResult.GetActor(), HitResult.GetComponent(), HitResult.Item, true, HitResult);
				}
			}

			LastTickOverlapComponentLocation = OverlapComponent->GetComponentLocation();
		}
	}
}
