// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameMode.h"

#include "EngineUtils.h"
#include "StormCore.h"
#include "Crunch/Network/CGameSession.h"
#include "Crunch/Player/CPlayerController.h"
#include "Crunch/Player/CPlayerState.h"
#include "GameFramework/PlayerStart.h"

class ACPlayerController;

ACGameMode::ACGameMode()
{
	GameSessionClass = ACGameSession::StaticClass();
}

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId TeamID = GetTeamIDForPlayer(NewPlayerController);
	
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);
	}
	//StartSpot : FindPlayerStart에서 StartSpot이 있으면 그 위치로 생성됨
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamID);
	
	return NewPlayerController;
}

void ACGameMode::StartPlay()
{
	Super::StartPlay();
	AStormCore* StormCore = GetStormCore();
	if (StormCore)
	{
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ACGameMode::MatchFinished);
	}
}

UClass* ACGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	ACPlayerState* PlayerState = InController->GetPlayerState<ACPlayerState>();
	if (PlayerState && PlayerState->GetSelectedPawnClass())
	{
		return PlayerState->GetSelectedPawnClass();
	}

	return BackupPawn;
}

APawn* ACGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
	FGenericTeamId TeamId = GetTeamIDForPlayer(NewPlayer);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}

	StartSpot = FindNextStartSpotForTeam(TeamId);
	NewPlayer->StartSpot = StartSpot;
	
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId ACGameMode::GetTeamIDForPlayer(const AController* InController) const
{
	ACPlayerState* PlayerState = InController->GetPlayerState<ACPlayerState>();
	if (PlayerState && PlayerState->GetSelectedPawnClass())
	{
		return PlayerState->GetTeamIdBasedOnSlot();
	}
	
	static int PlayerCount = 0;
	++PlayerCount;
	
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
	if (!StartSpotTag)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();

	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (It->PlayerStartTag == *StartSpotTag)
		{
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}
	
	return nullptr;
	
}

AStormCore* ACGameMode::GetStormCore() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AStormCore> It(World); It; ++It)
		{
			return *It;
		}
	}

	return nullptr;
}

void ACGameMode::MatchFinished(AActor* ViewTarget, int WiningTeam)
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ACPlayerController> It(World); It; ++It)
		{
			It->MatchFinished(ViewTarget, WiningTeam);
		}
	}

	
}
