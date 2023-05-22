

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SDANS_MontagePlayRate.generated.h"

/** Defines the ways that mods will modify value. */
UENUM(BlueprintType)
enum class EValueModOp : uint8
{
	Additive				UMETA(DisplayName = "Add"),
	Multiplicative		UMETA(DisplayName = "Multiply"),
	Override 			UMETA(DisplayName = "Override")
};

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDANS_MontagePlayRate : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	UPROPERTY(EditAnywhere, Category = Rate)
	EValueModOp RateScaleModOp = EValueModOp::Additive;

	UPROPERTY(EditAnywhere, Category = Rate)
	float Rate = 1.0f;

	float OriginRate = 1.0f;
};
