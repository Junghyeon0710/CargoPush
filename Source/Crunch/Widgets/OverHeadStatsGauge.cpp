// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadStatsGauge.h"

#include "ValueGauge.h"
#include "Crunch/GAS/CAttributeSet.h"

void UOverHeadStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* ASC)
{
	if (ASC)
	{
		HealthBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
	}
}
