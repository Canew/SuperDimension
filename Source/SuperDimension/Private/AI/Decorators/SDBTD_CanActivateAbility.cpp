


#include "AI/Decorators/SDBTD_CanActivateAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

USDBTD_CanActivateAbility::USDBTD_CanActivateAbility()
{
	NodeName = TEXT("Can Activate Ability");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;

	TSubclassOf<UGameplayAbility> SubclassOfGameplayAbility = UGameplayAbility::StaticClass();
	AbilityClassKey.AddClassFilter(this, AbilityClassKey.SelectedKeyName, SubclassOfGameplayAbility);
}

bool USDBTD_CanActivateAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return false;
	}

	USDAbilitySystemComponent* AbilitySystemComponent = Cast<USDAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	if (!AbilitySystemComponent)
	{
		return false;
	}

	if (TSubclassOf<UGameplayAbility> AbilityClass = AIController->GetBlackboardComponent()->GetValueAsClass(AbilityClassKey.SelectedKeyName))
	{
		if (FGameplayAbilitySpec* GameplayAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass, true))
		{
			if (UGameplayAbility* GameplayAbility = GameplayAbilitySpec->Ability)
			{
				if (GameplayAbility->CanActivateAbility(GameplayAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void USDBTD_CanActivateAbility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FBTDecorator_CanActivateAbility_Memory* DecoratorMemory = CastInstanceNodeMemory<FBTDecorator_CanActivateAbility_Memory>(NodeMemory);
	bool bCondition = CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bCondition != DecoratorMemory->BeforeData)
	{
		DecoratorMemory->BeforeData = bCondition;
		OwnerComp.RequestExecution(this);
	}
}

void USDBTD_CanActivateAbility::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	FBTDecorator_CanActivateAbility_Memory* DecoratorMemory = CastInstanceNodeMemory<FBTDecorator_CanActivateAbility_Memory>(NodeMemory);
	DecoratorMemory->BeforeData = CalculateRawConditionValue(OwnerComp, NodeMemory);
}

uint16 USDBTD_CanActivateAbility::GetInstanceMemorySize() const
{
	return sizeof(FBTDecorator_CanActivateAbility_Memory);
}

