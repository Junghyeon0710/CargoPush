// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsSelectionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTeamsSelectionWidget::SetSlotID(uint8 InSlotID)
{
	SlotID = InSlotID;
}

void UTeamsSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName)
{
	InfoText->SetText(FText::FromString(PlayerNickName));
}

void UTeamsSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectButton->OnClicked.AddDynamic(this, &ThisClass::SelectButtonClicked);
}

void UTeamsSelectionWidget::SelectButtonClicked()
{
	OnSlotClicked.Broadcast(SlotID);
}
