// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Crunch/Network/CNetStatics.h"

void UCGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		LoadLevelAndListen(GameLevel);
	}
}

void UCGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld())
		return;
	
	if (UCNetStatics::IsSessionServer(this))
	{
		CreateSession();
	}
}

void UCGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		ServerSessionName = UCNetStatics::GetSessionNameStr();
		FString SessionSearchId = UCNetStatics::GetSesisonSearchIdStr();
		SessionServerPort = UCNetStatics::GetSessionPort();

		FOnlineSessionSettings OnlineSessionSetting = UCNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, SessionServerPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSetting))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionSever();
		}
		UE_LOG(LogTemp, Warning, TEXT("Session Name: %s, ID: %s, Port: %d"), *ServerSessionName, *SessionSearchId, SessionServerPort);
	}
}

void UCGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("------------- Session Created!"))
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &UCGameInstance::WaitPlayerJoinTimeoutReached, WaitPlayerJoinTimeOutDuration);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("------------ Session Creation Failed"))
		TerminateSessionSever();
	}

	if (IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr())
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::EndSessisonCompleted(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::TerminateSessionSever()
{
	if (IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UCGameInstance::EndSessisonCompleted);
		if (!SessionPtr->EndSession(FName{ ServerSessionName }))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Sever shut down aftert %f seconds without player joining"), WaitPlayerJoinTimeOutDuration)
	TerminateSessionSever();
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));

	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Traveling to: %s"), *(TravelStr))
		GetWorld()->ServerTravel(TravelStr);
	}
}
