

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "SDHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERDIMENSION_API USDHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();

public:
	virtual void SetBossActor(AActor* InBossActor);

protected:
	virtual void InitializeHUD();
	virtual void OnPlayerHealthChanged(const FOnAttributeChangeData& Data);
	virtual void OnBossHealthChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* PlayerHealthBar;

	UPROPERTY()
	AActor* BossActor;

	UPROPERTY(Meta = (BindWidget))
	class UVerticalBox* BossHealthVerticalBox;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* BossNameText;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* BossHealthBar;
};
