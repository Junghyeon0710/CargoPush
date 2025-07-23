// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "StatsGauge.generated.h"

struct FOnAttributeChangeData;
class UTextBlock;
class UImage;

UCLASS()
class CRUNCH_API UStatsGauge : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeText;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;

	void SetValue(float NewVal);

	FNumberFormattingOptions NumberFormattingOptions;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};
