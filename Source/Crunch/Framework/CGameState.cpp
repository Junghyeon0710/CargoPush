// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ACGameState::RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot)
{
	if (!HasAuthority() || IsSlotOccupied(DesiredSlot))
		return;

	FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.IsForPlayer(RequestingPlayer);
		}
	);

	if (PlayerSelectionPtr)
	{
		PlayerSelectionPtr->SetSlot(DesiredSlot);
	}
	else
	{
		PlayerSelectionArray.Add(FPlayerSelection(DesiredSlot, RequestingPlayer));
	}

	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}

void ACGameState::SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefination* SelectedDefination)
{
	if (IsDefiniationSelected(SelectedDefination))
		return;

	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.IsForPlayer(SelectingPlayer);
	});

	if (FoundPlayerSelection)
	{
		FoundPlayerSelection->SetCharacterDefination(SelectedDefination);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

bool ACGameState::IsSlotOccupied(uint8 SlotId) const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
	{
		if (PlayerSelection.GetPlayerSlot() == SlotId)
		{
			return true;
		}
	}

	return false;
}

bool ACGameState::IsDefiniationSelected(const UPA_CharacterDefination* Defination) const
{
	const FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetCharacterDefination() == Defination;
		}
	);

	return FoundPlayerSelection != nullptr;
}

void ACGameState::SetCharacterDeSelected(const UPA_CharacterDefination* DefinationToDeSelect)
{
	if (!DefinationToDeSelect)
	{
		return;
	}
	
	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.GetCharacterDefination() == DefinationToDeSelect;
	});

	if (FoundPlayerSelection)
	{
		FoundPlayerSelection->SetCharacterDefination(nullptr);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

void ACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ACGameState, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
}

const TArray<FPlayerSelection>& ACGameState::GetPlayerSelection() const
{
	return PlayerSelectionArray;
}

bool ACGameState::CanStartHeroSelection() const
{
	return PlayerSelectionArray.Num() == PlayerArray.Num();
}

void ACGameState::OnRep_PlayerSelectionArray()
{
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}
