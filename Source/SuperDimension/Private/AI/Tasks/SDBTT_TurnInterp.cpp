// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SDBTT_TurnInterp.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USDBTT_TurnInterp::USDBTT_TurnInterp()
{
	NodeName = FString(TEXT("Turn Interp"));
	TurnInterpSpeed = 30.0f;
}

EBTNodeResult::Type USDBTT_TurnInterp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	FRotator PawnToTargetRotator = (TargetActor->GetActorLocation() - Pawn->GetActorLocation()).Rotation();
	PawnToTargetRotator.Roll = 0.0f;
	PawnToTargetRotator.Pitch = 0.0f;
	FRotator NewRotation = FMath::RInterpConstantTo(Pawn->GetActorRotation(), PawnToTargetRotator, Pawn->GetWorld()->GetDeltaSeconds(), TurnInterpSpeed);
	Pawn->SetActorRotation(NewRotation);

	return EBTNodeResult::Succeeded;
}
