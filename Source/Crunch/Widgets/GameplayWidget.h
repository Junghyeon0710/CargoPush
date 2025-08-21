// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crunch/GAS/CGameAbilityTypes.h"
#include "GameplayWidget.generated.h"

class UStatsGauge;
class UAbilityListView;
class UGameplayAbility;
class UValueGauge;
class UAbilitySystemComponent;
class UShopWidget;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);

	void ToggleShop();

	UFUNCTION()
	void ToggleGameplayMenu();

	void ShowGameplayMenu();
	void SetGameplayMenuTitle(const FString& NewTitle);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> AttackDamageGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> ArmorGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> MoveSpeedGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> IntelligenceGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> StrengthGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UShopWidget> ShopWidget;

	UPROPERTY(meta=(BindWidget))
	class UInventoryWidget* InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	class USkeletalMeshRenderWidget* HeadshotWidget;

	UPROPERTY(meta=(BindWidget))
	class UMatchStatWidget* MatchStatWidget;

	UPROPERTY(meta=(BindWidget))
	class UGameplayMenu* GameplayMenu;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* GameplayWidgetRootPanel;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* GameplayMenuRootPanel;

	UPROPERTY(meta=(BindWidget))
	class UCrosshairWidget* CrosshairWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* ShopPopupAnimation;

	void PlayShopPopupAnimation(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bPawnInputEnabled);
	void SetShowMouseCursor(bool bShowMouseCursor);
	void SetFocusToGameAndUI();
	void SetFocusToGameOnly();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};
