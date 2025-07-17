// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityGauge : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownCounterText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CostText;
};
