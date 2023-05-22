// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "SDAIController.generated.h"

enum class EAIBehaviorMode : uint8
{
	Idle,
	Aggressive,
	Defensive,
	Evasive
};

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API ASDAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASDAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn);

	void RunAI();
	void RestartAI();
	void StopAI();

	void PredictBeingHit();
	bool IsAggressive() const;

protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void InitializeHomeLocation();
	virtual void TargetClosestHostileActor();
	virtual void ExpireTarget(AActor* Actor, FAIStimulus Stimulus);
	virtual void SetVisibilityPawnHealthToTarget(AActor* Target, bool bVisible);

protected:
	UPROPERTY()
	class UAISenseConfig_Sight* SightSenseConfig;
	UPROPERTY()
	class UAISenseConfig_Damage* DamageSenseConfig;
	UPROPERTY()
	class UAISenseConfig_Hearing* HearingSenseConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBlackboardData* BlackboardData;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	FTimerHandle TargetExpirationTimerHandle;
	FName HomeLocationKeyName;
	FName TargetActorKeyName;

	float SmoothFocusInterpSpeed;

	FTimerHandle ResetPredictBeingHitValueTimerHandle;
};
