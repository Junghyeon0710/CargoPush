// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Crunch/Player/CPlayerState.h"
#include "Crunch/Framework/CGameState.h"

void ALobbyPlayerController::Server_RequestSlotSelectionChange_Implementation(uint8 NewSlotID)
{
	if (!GetWorld())
		return;

	ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
	if (!CGameState)
		return;

	CGameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotID);
} 

bool ALobbyPlayerController::Server_RequestSlotSelectionChange_Validate(uint8 NewSlotID)
{
	return true;
}

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld())
		return;

	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(*PlayerControllerIterator);
		if (LobbyPlayerController)
		{
			LobbyPlayerController->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelection.ExecuteIfBound();
}
