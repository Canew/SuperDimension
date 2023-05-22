
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SDGT_WaitOverlapEvent.generated.h"

/** Delegate for notification of start of overlap with a specific component */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FComponentBeginOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult&, SweepResult);

/** Structure that defines parameters for collision event */
USTRUCT(BlueprintType)
struct FWaitOverlapEventParams
{
	GENERATED_BODY()

	/** Location offset from the socket */
	UPROPERTY(BlueprintReadWrite)
	FVector LocationOffset;

	/** Rotation offset from the socket */
	UPROPERTY(BlueprintReadWrite)
	FRotator RotationOffset;

	/** Socket name to attach overlap component */
	UPROPERTY(BlueprintReadWrite)
	FName CollisionAttachSocketName;

	/** Profile name to be used for collision */
	UPROPERTY(BlueprintReadWrite)
	FName CollisionProfileName;

	/** Whether overlap component should be attached */
	UPROPERTY(BlueprintReadWrite)
	bool bAttachToAvatarActor;

	/** Whether end task when occurs overlap once */
	UPROPERTY(BlueprintReadWrite)
	bool bOverlapOnlyOnce;

	/** Whether overlap component should be visible */
	UPROPERTY(BlueprintReadWrite)
	bool bDebug;

	FWaitOverlapEventParams()
		: LocationOffset(FVector::ZeroVector), RotationOffset(FRotator::ZeroRotator),
		CollisionAttachSocketName(NAME_None), CollisionProfileName(NAME_None),
		bAttachToAvatarActor(true), bOverlapOnlyOnce(false), bDebug(false)
	{}
};

/**
 * Ability task for overlap event
 */
UCLASS()
class SUPERDIMENSION_API USDGT_WaitOverlapEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	USDGT_WaitOverlapEvent();

	virtual void Activate() override;

	/** Tick function for this task, if bTickingTask == true */
	virtual void TickTask(float DeltaTime) override;

	/**
	  * Sweep a shape against the world and broadcast all initial overlaps using a specific collision profile
	  * 
	  * @param TaskInstanceName Set to override the name of this task, for later querying
	  * @param CollisionShape collision shape used for overelap, supports Box, Sphere, Capsule
	  * @param WaitOverlapEventParams Additional parameters used for the overlap task
	  */
	static USDGT_WaitOverlapEvent* CreateWaitOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName,
		FCollisionShape CollisionShape, FWaitOverlapEventParams WaitOverlapEventParams);

	/**
	  * Sweep a shape against the world and broadcast all initial overlaps using a specific collision profile
	  *
	  * @param TaskInstanceName Set to override the name of this task, for later querying
	  * @param BoxHalfExtent Box half extent of box collision shape
	  * @param WaitOverlapEventParams Additional parameters used for the overlap task
	  */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USDGT_WaitOverlapEvent* CreateWaitBoxOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName,
		FVector BoxHalfExtent, FWaitOverlapEventParams WaitOverlapEventParams);

	/**
	  * Sweep a shape against the world and broadcast all initial overlaps using a specific collision profile
	  *
	  * @param TaskInstanceName Set to override the name of this task, for later querying
	  * @param Radius radius of sphere collision shape
	  * @param WaitOverlapEventParams Additional parameters used for the overlap task
	  */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USDGT_WaitOverlapEvent* CreateWaitSphereOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName,
		float Radius, FWaitOverlapEventParams WaitOverlapEventParams);

	/**
	  * Sweep a shape against the world and broadcast all initial overlaps using a specific collision profile
	  *
	  * @param TaskInstanceName Set to override the name of this task, for later querying
	  * @param Radius Radius of capsule collision shape
	  * @param HalfHeight HalfHeight of capsule collision shape
	  * @param WaitOverlapEventParams Additional parameters used for the overlap task
	  */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USDGT_WaitOverlapEvent* CreateWaitCapsuleOverlapEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName,
		float Radius, float HalfHeight, FWaitOverlapEventParams WaitOverlapEventParams);

	void AddIgnoreActor(AActor* IgnoreActor);
	void AddIgnoreActors(TArray<AActor*> IgnoreActors);

	UPrimitiveComponent* GetOverlapComponent() const;

private:
	virtual void OnDestroy(bool AbilityEnded) override;

	virtual UPrimitiveComponent* FindAttachComponent();

	/** Do sweep from last tick location */
	virtual void SweepOverlapComponent();

public:
	UPROPERTY(BlueprintAssignable)
	FComponentBeginOverlap OnOverlap;

	/** Time span between subsequent ticks of the task, if TickInterval == 0.0f execute every ticks */
	UPROPERTY(BlueprintReadWrite)
	float TickInterval;

private:
	UPROPERTY()
	UPrimitiveComponent* OverlapComponent;
	FCollisionShape OverlapCollisionShape;

	FWaitOverlapEventParams WaitOverlapEventParams;
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> OverlapIgnoreActors;
	FVector LastTickOverlapComponentLocation;

	float AccumulatedDeltaTime;
};
