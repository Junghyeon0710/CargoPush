// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "LobbyPlayerController.generated.h"

DECLARE_DELEGATE(FOnSwitchToHeroSelection);

UCLASS()
class CRUNCH_API ALobbyPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()
public:
	FOnSwitchToHeroSelection OnSwitchToHeroSelection;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSlotSelectionChange(uint8 NewSlotID);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartHeroSelection();

	UFUNCTION(Client, Reliable)
	void Client_StartHeroSelection();
};
