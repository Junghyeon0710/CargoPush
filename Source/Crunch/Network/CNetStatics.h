// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CNetStatics.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static IOnlineSessionPtr GetSessionPtr();
	static IOnlineIdentityPtr GetIdentityPtr();
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int32 Port);
	
	static uint8 GetPlayerCountPerTeam();

	static bool IsSessionServer(const UObject* WorldContextObject);

	static FString GetSessionNameStr();
	static FName GetSessionNameKey();

	static FString GetSesisonSearchIdStr();
	static FName GetSessionSearchIdKey();

	static int GetSessionPort();
	static FName GetPortKey();

	static FString GetCommandlineArgAsString(const FName& ParamName);
	static int GetCommandlineArgAsInt(const FName& ParamName);
	
};
