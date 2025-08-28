// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamsSelectionWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /*SlotID*/);

UCLASS()
class CRUNCH_API UTeamsSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetSlotID(uint8 InSlotID);
	void UpdateSlotInfo(const FString& PlayerNickName);
	virtual void NativeConstruct() override;

	FOnSlotClicked OnSlotClicked;

private:
	UPROPERTY(meta=	(BindWidget))
	class UButton* SelectButton;

	UPROPERTY(meta=	(BindWidget))
	class UTextBlock* InfoText;

	UFUNCTION()
	void SelectButtonClicked();

	uint8 SlotID;
};
