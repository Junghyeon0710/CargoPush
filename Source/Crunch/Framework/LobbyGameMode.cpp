// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "Crunch/Network/CGameSession.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = ACGameSession::StaticClass();
}
