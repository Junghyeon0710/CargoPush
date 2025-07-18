// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "Crunch/GAS/CGameAbilityTypes.h"
#include "AbilityListView.generated.h"

class UGameplayAbility;

UCLASS()
class CRUNCH_API UAbilityListView : public UListView
{
	GENERATED_BODY()
public:
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);
	
};
