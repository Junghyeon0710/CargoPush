// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.generated.h"

class ACGameState;

UCLASS()
class CRUNCH_API ACPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ACPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacterDefination(const UPA_CharacterDefination* NewDefination);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;

	UPROPERTY()
	class ACGameState* CGameState;

	void PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections);
};
