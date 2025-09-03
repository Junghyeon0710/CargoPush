// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void StartMatch();
	virtual void Init() override;

	/********************************/
	/*          Session Server      */
	/********************************/

	void CreateSession();
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	void EndSessisonCompleted(FName SessionName, bool bWasSuccessful);
	FString ServerSessionName;
	int SessionServerPort;

	void TerminateSessionSever();

	FTimerHandle WaitPlayerJoinTimeoutHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float WaitPlayerJoinTimeOutDuration = 60.f;

	void WaitPlayerJoinTimeoutReached();
	
private:	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;

	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};
