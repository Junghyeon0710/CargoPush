// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;
class UGameplayAbility;
class UTextBlock;
class UImage;
struct FGameplayAbilitySpec;

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};


UCLASS()
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CooldownPercentParaName = "Percent";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName AbilityLevelParmName = "Level";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CanCastAbilityParamName = "CanCast";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName UpgradePointAvailableParamName = "UpgradeAvailable";

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> LevelGauge;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownCounterText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CostText;

	UPROPERTY()
	TObjectPtr<UGameplayAbility> AbilityCDO;

	void AbilityCommitted(UGameplayAbility* Ability);

	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);

	float CachedCooldownDuration;
	float CachedCooldownTimeRemaining;

	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdatedHandle;

	FNumberFormattingOptions WholeNumberFormationOptions;
	FNumberFormattingOptions TwoDigitNumberFormationOptions;

	void CooldownFinished();
	void UpdateCooldown();
	
	const UAbilitySystemComponent* OwnerAbilitySystemComponet;
	const FGameplayAbilitySpec* CachedAbilitySpec;

	const FGameplayAbilitySpec* GetAbilitySpec();

	bool bIsAbilityLeaned = false;

	void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);
	void UpdatedCanCast();
	void UpgradePointUpdated(const FOnAttributeChangeData& Data);
};
