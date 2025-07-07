// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//서버에서만 호출
	virtual void OnPossess(APawn* InPawn) override;
	// 클라에서만 호출
	virtual void AcknowledgePossession(class APawn* P) override;
	
private:
	void SpawnGameplayWidget();
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;
};
