// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameInstance.h"

#include "HttpModule.h"
#include "OnlineSessionSettings.h"
#include "Crunch/Network/CNetStatics.h"
#include "Interfaces/IHttpRequest.h"

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

bool UCGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr())
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}

	return false;
}

bool UCGameInstance::IsLoggingIn() const
{
	return LoggingInDelegateHandle.IsValid();
}

void UCGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UCGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UCGameInstance::LoginCompleted);
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed Right Away!"))
			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
				LoggingInDelegateHandle.Reset();
			}
			OnLoginCompleted.Broadcast(false, "", "Login Failed Right Away!");
		}
	}
}

void UCGameInstance::LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		FString PlayerNickname = "";
		if (bWasSuccessful)
		{
			PlayerNickname = IdentityPtr->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("Logged in succesfully as: %s"), *(PlayerNickname))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Loging in failed: %s"), *(Error))
		}

		OnLoginCompleted.Broadcast(bWasSuccessful, PlayerNickname, Error);
	}
	else
	{
		OnLoginCompleted.Broadcast(false, "", "Can't find the Identity Pointer");
	}
}

void UCGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating and Joining Session %s"), *NewSessionName.ToString());
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchGuid = FGuid::NewGuid();

	FString CoordinatorURL = UCNetStatics::GetCoordinatorURL();
	UE_LOG(LogTemp, Warning, TEXT("Coordinator URL: %s"), *CoordinatorURL);
}

void UCGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("Cancelling Session Creation"))
}

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}

	PlayerRecord.Add(UniqueId);
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All player left the session, terminating"))
		TerminateSessionServer();
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
			TerminateSessionServer();
		}
		UE_LOG(LogTemp, Warning, TEXT("Session Name: %s, ID: %s, Port: %d"), *ServerSessionName, *SessionSearchId, SessionServerPort);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find sesison ptr, terminating"))
		TerminateSessionServer();
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
		TerminateSessionServer();
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

void UCGameInstance::TerminateSessionServer()
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
	TerminateSessionServer();
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
