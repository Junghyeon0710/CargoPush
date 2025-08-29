// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "TeamsSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Crunch/Character/PA_CharacterDefination.h"
#include "Crunch/Framework/CAssetManager.h"
#include "Crunch/Framework/CGameState.h"
#include "Crunch/Network/CNetStatics.h"
#include "Crunch/Player/LobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ClearAndPopulateTeamSelectionSlots();
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	ConfigureGameState();
	if (LobbyPlayerController)
	{
		LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ULobbyWidget::SwitchToHeroSelection);
	}
	StartHeroSelectionButton->SetIsEnabled(false);
	StartHeroSelectionButton->OnClicked.AddDynamic(this, &ThisClass::StartHeroSelectionButtonClicked);

	UCAssetManager::Get().LoadCharacterDefinations(FStreamableDelegate::CreateUObject(this, &ThisClass::CharacterDefinitionLoaded));
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	for (int i = 0; i < UCNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		UTeamsSelectionWidget* NewSelectionSlot = CreateWidget<UTeamsSelectionWidget>(this, TeamSelectionWidgetClass);
		if (NewSelectionSlot)
		{
			NewSelectionSlot->SetSlotID(i);
			UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot);
			if (NewGridSlot)
			{
				int Row = i % UCNetStatics::GetPlayerCountPerTeam();
				int Column = i < UCNetStatics::GetPlayerCountPerTeam() ? 0 : 1;

				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Column);
			}

			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ULobbyWidget::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID)
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestSlotSelectionChange(NewSlotID);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	CGameState = World->GetGameState<ACGameState>();
	if (!CGameState)
	{
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ULobbyWidget::ConfigureGameState, 1.f);
	}
	else
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ULobbyWidget::UpdatePlayerSelectionDisplay);
		UpdatePlayerSelectionDisplay(CGameState->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UTeamsSelectionWidget* SelectionSlot : TeamSelectionSlots)
	{
		SelectionSlot->UpdateSlotInfo("Empty");
	}

	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
			continue;

		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());
	}

	if (CGameState)
	{
		StartHeroSelectionButton->SetIsEnabled(CGameState->CanStartHeroSelection());
	}
}

void ULobbyWidget::StartHeroSelectionButtonClicked()
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection()
{
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::CharacterDefinitionLoaded()
{
	TArray<UPA_CharacterDefination*> LoadedCharacterDefinations;
	if (UCAssetManager::Get().GetLoadedCharacterDefinations(LoadedCharacterDefinations))
	{
		CharacterSelectionTileView->SetListItems(LoadedCharacterDefinations);
	}
	
}
