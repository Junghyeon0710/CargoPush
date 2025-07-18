// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityListView.h"
#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Ability : Abilities)
	{
		AddItem(Ability.Value.GetDefaultObject());
	}
}
