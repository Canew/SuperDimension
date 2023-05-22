// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayEffectTypes.h"
#include "SDANS_ApplyGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDANS_ApplyGameplayEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	FString GetNotifyName_Implementation() const override;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

protected:
	UPROPERTY(EditAnywhere, Category = "GameplayEffect")
	TArray<TSubclassOf<class UGameplayEffect>> GameplayEffectClasses;

	TArray<FActiveGameplayEffectHandle> ActiveGameplayEffectHandles;
};
