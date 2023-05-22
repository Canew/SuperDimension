

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SDGT_WaitSpeedChange.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpeedChangeDelegate);

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDGT_WaitSpeedChange : public UAbilityTask
{
	GENERATED_BODY()

public:
	USDGT_WaitSpeedChange();

	virtual void Activate() override;

	/** Tick function for this task, if bTickingTask == true */
	virtual void TickTask(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USDGT_WaitSpeedChange* CreateWaitSpeedChangeEvent(UGameplayAbility* OwningAbility, float MinSpeed);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSpeedChangeDelegate OnChange;

protected:
	float MinSpeed;
};
