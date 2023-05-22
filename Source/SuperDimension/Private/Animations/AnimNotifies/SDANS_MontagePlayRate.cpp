


#include "Animations/AnimNotifies/SDANS_MontagePlayRate.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

FString USDANS_MontagePlayRate::GetNotifyName_Implementation() const
{
	return TEXT("MontageRateScale");
}

void USDANS_MontagePlayRate::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (Character == nullptr)
	{
		return;
	}

	UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation);
	if (AnimMontage == nullptr)
	{
		return;
	}

	OriginRate = AnimMontage->RateScale;
	float FinalPlayRate = 1.0f;
	switch (RateScaleModOp)
	{
	case EValueModOp::Additive:
		FinalPlayRate = OriginRate + Rate;
		break;

	case EValueModOp::Multiplicative:
		FinalPlayRate = OriginRate * Rate;
		break;

	case EValueModOp::Override:
		FinalPlayRate = OriginRate = Rate;
		break;

	default:
		break;
	}

	if (AnimMontage)
	{
		Character->GetMesh()->GetAnimInstance()->Montage_SetPlayRate(AnimMontage, FinalPlayRate);
	}
}

void USDANS_MontagePlayRate::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void USDANS_MontagePlayRate::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (Character == nullptr)
	{
		return;
	}

	UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation);
	if (AnimMontage)
	{
		Character->GetMesh()->GetAnimInstance()->Montage_SetPlayRate(AnimMontage, OriginRate);
	}
}
