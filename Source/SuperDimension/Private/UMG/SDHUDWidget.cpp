


#include "UMG/SDHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Abilities/SDAttributeSet.h"
#include "Abilities/SDAbilitySystemComponent.h"
#include "SDCharacter.h"

void USDHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ASDCharacter* OwningCharacter = GetOwningPlayerPawn<ASDCharacter>())
	{
		UAbilitySystemComponent* AbilitySystemComponent = OwningCharacter->GetAbilitySystemComponent();
		USDAttributeSet* AttributeSet = OwningCharacter->GetAttributeSet<USDAttributeSet>();
		if (AbilitySystemComponent && AttributeSet)
		{
			InitializeHUD();
			SetBossActor(nullptr);

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &USDHUDWidget::OnPlayerHealthChanged);
		}
	}
}

void USDHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void USDHUDWidget::SetBossActor(AActor* InBossActor)
{
	BossActor = InBossActor;
	if (!BossActor)
	{
		BossHealthVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (ASDCharacter* BossCharacter = Cast<ASDCharacter>(BossActor))
	{
		UAbilitySystemComponent* AbilitySystemComponent = BossCharacter->GetAbilitySystemComponent();
		USDAttributeSet* AttributeSet = BossCharacter->GetAttributeSet<USDAttributeSet>();
		if (AbilitySystemComponent && AttributeSet)
		{
			BossHealthVerticalBox->SetVisibility(ESlateVisibility::HitTestInvisible);

			FName BossNickname = BossCharacter->GetNickname();
			BossNickname == NAME_None ? BossNameText->SetText(FText::FromString(TEXT(""))) : BossNameText->SetText(FText::FromString(BossCharacter->GetNickname().ToString()));

			float CurrentHealth = AttributeSet->GetHealth();
			float MaxHealth = AttributeSet->GetMaxHealth();
			BossHealthBar->SetPercent(CurrentHealth / MaxHealth);

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &USDHUDWidget::OnBossHealthChanged);
		}
	}
}

void USDHUDWidget::InitializeHUD()
{
	if (ASDCharacter* OwningCharacter = GetOwningPlayerPawn<ASDCharacter>())
	{
		USDAttributeSet* AttributeSet = OwningCharacter->GetAttributeSet<USDAttributeSet>();
		if (AttributeSet)
		{
			float CurrentHealth = AttributeSet->GetHealth();
			float MaxHealth = AttributeSet->GetMaxHealth();
			PlayerHealthBar->SetPercent(CurrentHealth / MaxHealth);
		}
	}
}

void USDHUDWidget::OnPlayerHealthChanged(const FOnAttributeChangeData& Data)
{
	if (ASDCharacter* OwningCharacter = GetOwningPlayerPawn<ASDCharacter>())
	{
		USDAttributeSet* AttributeSet = OwningCharacter->GetAttributeSet<USDAttributeSet>();
		if (AttributeSet)
		{
			float CurrentHealth = Data.NewValue;
			float MaxHealth = AttributeSet->GetMaxHealth();
			PlayerHealthBar->SetPercent(CurrentHealth / MaxHealth);
		}
	}
}

void USDHUDWidget::OnBossHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!BossActor)
	{
		return;
	}

	if (ASDCharacter* BossCharacter = Cast<ASDCharacter>(BossActor))
	{
		USDAttributeSet* AttributeSet = BossCharacter->GetAttributeSet<USDAttributeSet>();
		if (AttributeSet)
		{
			float CurrentHealth = Data.NewValue;
			float MaxHealth = AttributeSet->GetMaxHealth();
			BossHealthBar->SetPercent(CurrentHealth / MaxHealth);
		}
	}
}