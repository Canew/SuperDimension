// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/SDAN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

FString USDAN_SendGameplayEvent::GetNotifyName_Implementation() const
{
	return FString(TEXT("SendGameplayEvent"));
}

void USDAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Actor = MeshComp->GetOwner();
	if (Actor)
	{
		FGameplayEventData GameplayEventData;
		GameplayEventData.Instigator = Actor;
		GameplayEventData.EventTag = SendGameplayTag;
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
		{
			GameplayEventData.ContextHandle = AbilitySystemComponent->MakeEffectContext();
		}

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, SendGameplayTag, GameplayEventData);
	}
}

