// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/GAS/UCAbilitySystemStatics.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommitted);
	}

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
