// Fill out your copyright notice in the Description page of Project Settings.


#include "ValueGauge.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(BarColor);

	ValueText->SetFont(ValueTextFont);
	ValueText->SetVisibility(bValueTextVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	ProgressBar->SetVisibility(bProgressBarVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	
}

void UValueGauge::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComp, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	if (AbilitySystemComp)
	{
		bool bFound;
		float Value = AbilitySystemComp->GetGameplayAttributeValue(Attribute, bFound);
		float MaxValue = AbilitySystemComp->GetGameplayAttributeValue(MaxAttribute, bFound);
		if (bFound)
		{
			SetValue(Value, MaxValue);
		}

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::ValueChanged);
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &ThisClass::MaxValueChanged);
	}
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;
	if (NewMaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value is zero"));
		return;
	}

	float NewPercent = NewValue / NewMaxValue;
	ProgressBar->SetPercent(NewPercent);

	FNumberFormattingOptions FormatOps = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),
			FText::AsNumber(NewValue, &FormatOps),
			FText::AsNumber(NewMaxValue, &FormatOps)
			)
		);
}

void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(ChangeData.NewValue, CachedMaxValue);
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(CachedValue, ChangeData.NewValue);
	
}
