// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SDBTT_TurnInterp.generated.h"

/**
 *
 */
UCLASS()
class SUPERDIMENSION_API USDBTT_TurnInterp : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	USDBTT_TurnInterp();

	/** @return name of node */
	FString GetNodeName() const;

	/** starts this task, should return Succeeded, Failed or InProgress
 *  (use FinishLatentTask() when returning InProgress)
 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere)
	float TurnInterpSpeed;
};
