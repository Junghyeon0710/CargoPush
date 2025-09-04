// Fill out your copyright notice in the Description page of Project Settings.


#include "CNetStatics.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

IOnlineSessionPtr UCNetStatics::GetSessionPtr()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		return OnlineSubsystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UCNetStatics::GetIdentityPtr()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		return OnlineSubsystem->GetIdentityInterface();
	}
	return nullptr;
}

FOnlineSessionSettings UCNetStatics::GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int32 Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false;
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = false;
	OnlineSessionSettings.bAllowJoinInProgress = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;

	OnlineSessionSettings.Set<FString>(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set<FString>(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	return OnlineSessionSettings;
}

uint8 UCNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UCNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
}

FString UCNetStatics::GetSessionNameStr()
{
	return GetCommandlineArgAsString(GetSessionNameKey());
}

FName UCNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UCNetStatics::GetSesisonSearchIdStr()
{
	return GetCommandlineArgAsString(GetSessionSearchIdKey());
}

FName UCNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UCNetStatics::GetSessionPort()
{
	return GetCommandlineArgAsInt(GetPortKey());
}

FName UCNetStatics::GetPortKey()
{
	return FName("PORT");
}

FName UCNetStatics::GetCoordinatorURLKey()
{
	return FName("COORDINATOR_URL");
}

FString UCNetStatics::GetCoordinatorURL()
{
	FString CoordinatorURL = GetCommandlineArgAsString(GetCoordinatorURLKey());
	if (CoordinatorURL.IsEmpty())
	{
		CoordinatorURL = GetDefaultCoordinatorURL();
	}
	return CoordinatorURL;
}

FString UCNetStatics::GetDefaultCoordinatorURL()
{
	FString CoordinatorURL = "";
	GConfig->GetString(TEXT("Crunch.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	return CoordinatorURL;
}

FString UCNetStatics::GetCommandlineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("/%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UCNetStatics::GetCommandlineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("/%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}
