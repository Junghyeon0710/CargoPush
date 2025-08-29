// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crunch/Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* TeamsSelectionRoot;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartHeroSelectionButton;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* TeamSelectionSlotGridPanel;

	UPROPERTY(EditDefaultsOnly, Category = "TeamSelection")
	TSubclassOf<class UTeamsSelectionWidget> TeamSelectionWidgetClass;

	UPROPERTY()
	TArray<UTeamsSelectionWidget*> TeamSelectionSlots;

	void ClearAndPopulateTeamSelectionSlots();
	void SlotSelected(uint8 NewSlotID);

	UPROPERTY(meta = (BindWidget))
	class UWidget* HeroSelectionRoot;
	
	UPROPERTY()
	class ALobbyPlayerController* LobbyPlayerController;

	void ConfigureGameState();
	FTimerHandle ConfigureGameStateTimerHandle;

	UPROPERTY()
	class ACGameState* CGameState;


	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);
	UFUNCTION()
	void StartHeroSelectionButtonClicked();

	void SwitchToHeroSelection();
	void CharacterDefinitionLoaded();
};
