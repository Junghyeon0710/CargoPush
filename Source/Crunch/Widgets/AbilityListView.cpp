// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityListView.h"

#include "AbilityGauge.h"
#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	OnEntryWidgetGenerated().AddUObject(this, &ThisClass::AbilityGaugeGenerated);
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Ability : Abilities)
	{
		AddItem(Ability.Value.GetDefaultObject());
	}
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);

	if (AbilityGauge)
	{
		AbilityGauge->ConfigureWithWidgetData(FindWidgetDataForAbility(AbilityGauge->GetListItem<UGameplayAbility>()->GetClass()));
	}
}

const FAbilityWidgetData* UAbilityListView::FindWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!AbilityDataTable)
	{
		return nullptr;
	}

	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		const FAbilityWidgetData* WidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityWidgetDataPair.Key,"");
		if (WidgetData->AbilityClass == AbilityClass)
		{
			return WidgetData;
		}
	}
	
	return nullptr;
}
