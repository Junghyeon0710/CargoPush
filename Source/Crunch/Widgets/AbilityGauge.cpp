// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/GAS/CAttributeSet.h"
#include "Crunch/GAS/CHeroAttributeSet.h"
#include "Crunch/GAS/UCAbilitySystemStatics.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommitted);
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &ThisClass::AbilitySpecUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &ThisClass::UpgradePointUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);
		
		bool bFound = false;
		float UpgradePoint = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetUpgradePointAttribute(), bFound);
		if (bFound)
		{
			FOnAttributeChangeData ChangeData;
			ChangeData.NewValue = UpgradePoint;
			UpgradePointUpdated(ChangeData);
		}
	}
	
	OwnerAbilitySystemComponet = OwnerASC;
	WholeNumberFormationOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormationOptions.MaximumFractionalDigits = 2;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
	float Cost = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

	CostText->SetText(FText::AsNumber(Cost));
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParmName, 0);
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
	}
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.f;
		float CooldownDuration = 0.f;

		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);

		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CachedCooldownDuration = CooldownDuration;
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CachedCooldownTimeRemaining = CooldownTimeRemaining;

	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);
	
	
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ThisClass::CooldownFinished, CooldownTimeRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdatedHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true, 0.f);
}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.f;
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParaName, 1.f);
	
}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;

	FNumberFormattingOptions* FormattingOptions = CachedCooldownTimeRemaining > 1 ? &WholeNumberFormationOptions : &TwoDigitNumberFormationOptions;
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, FormattingOptions));

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParaName, 1.f - CachedCooldownTimeRemaining / CachedCooldownDuration);
}

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	if (!CachedAbilitySpec)
	{
		if (AbilityCDO && OwnerAbilitySystemComponet)
		{
			CachedAbilitySpec = OwnerAbilitySystemComponet->FindAbilitySpecFromClass(AbilityCDO->GetClass());
		}
	}

	return CachedAbilitySpec;
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability != AbilityCDO)
	{
		return;
	}

	bIsAbilityLeaned = AbilitySpec.Level > 0;
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParmName, AbilitySpec.Level);
	UpdatedCanCast();

	float NewCooldownDuration = UCAbilitySystemStatics::GetCooldownDurationFor(AbilitySpec.Ability, *OwnerAbilitySystemComponet, AbilitySpec.Level);
	float NewCost = UCAbilitySystemStatics::GetManaCostFor(AbilitySpec.Ability, *OwnerAbilitySystemComponet, AbilitySpec.Level);
	CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdatedCanCast()
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	bool bCanCast = bIsAbilityLeaned;
	if (AbilitySpec)
	{
		if (OwnerAbilitySystemComponet && !UCAbilitySystemStatics::CheckAbilityCost(*AbilitySpec, *OwnerAbilitySystemComponet))
		{
			bCanCast = false;
		}
	}

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bCanCast ? 1 : 0);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& Data)
{
	bool HasUpgradePoint = Data.NewValue >0;
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (AbilitySpec)
	{
		if (UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, 0);
			return;
		}
	}
	Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, HasUpgradePoint ? 1 : 0);
	
}

void UAbilityGauge::ManaUpdated(const FOnAttributeChangeData& Data)
{
	UpdatedCanCast();
}

