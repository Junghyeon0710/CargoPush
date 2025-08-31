// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTeamLayoutWidget.h"

#include "PlayerTeamSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Crunch/Network/CNetStatics.h"

void UPlayerTeamLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TeamOneLayoutBox->ClearChildren();
	TeamTwoLayoutBox->ClearChildren();
	
	if (!PlayerTeamSlotWidgetClass)
		return;

	for (int i = 0; i < UCNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		UPlayerTeamSlotWidget* NewSlotWidget = CreateWidget<UPlayerTeamSlotWidget>(GetOwningPlayer(), PlayerTeamSlotWidgetClass);
		TeamSlotWidgets.Add(NewSlotWidget);
		
		UHorizontalBoxSlot* NewSlot;
		if (i < UCNetStatics::GetPlayerCountPerTeam())
		{
			NewSlot = TeamOneLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
		}
		else
		{
			NewSlot = TeamTwoLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
		}

		NewSlot->SetPadding(FMargin{ PlayerTeamWidgetSlotMargin });
	}
}

void UPlayerTeamLayoutWidget::UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UPlayerTeamSlotWidget* SlotWidget : TeamSlotWidgets)
	{
		SlotWidget->UpateSlot("", nullptr);
	}

	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
			continue;

		TeamSlotWidgets[PlayerSelection.GetPlayerSlot()]->UpateSlot(PlayerSelection.GetPlayerNickName(), PlayerSelection.GetCharacterDefination());
	}
}