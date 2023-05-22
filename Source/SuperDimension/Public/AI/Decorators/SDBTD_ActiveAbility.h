// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SDBTD_ActiveAbility.generated.h"

/**
 *
 */
UCLASS()
class SUPERDIMENSION_API USDBTD_ActiveAbility : public UBTDecorator
{
	GENERATED_BODY()

public:
	USDBTD_ActiveAbility();

protected:
	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Ability")
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, Category = "Ability")
	bool bFindChildClass;
};
