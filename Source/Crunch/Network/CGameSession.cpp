// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameSession.h"
#include "Crunch/Framework/CGameInstance.h"

bool ACGameSession::ProcessAutoLogin()
{
	return true;
}

void ACGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (UCGameInstance* GameInst = GetGameInstance<UCGameInstance>())
	{
		GameInst->PlayerJoined(UniqueId);
	}
}

void ACGameSession::UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(FromSessionName, UniqueId);
	if (UCGameInstance* GameInst = GetGameInstance<UCGameInstance>())
	{
		GameInst->PlayerLeft(UniqueId);
	}
}