// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UGameplayWidget;
class ACPlayerCharacter;
class UInputMappingContext;
class UInputAction;

UCLASS()
class CRUNCH_API ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	//서버에서만 호출
	virtual void OnPossess(APawn* InPawn) override;
	// 클라에서만 호출
	virtual void AcknowledgePossession(class APawn* P) override;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupInputComponent() override;
	void MatchFinished(AActor* ViewTarget, int WiningTeam);
	
private:
	UFUNCTION(Client, Reliable)
	void Client_MatchFinished(AActor* ViewTarget, int WiningTeam);
	
	void SpawnGameplayWidget();

	UPROPERTY(EditDefaultsOnly, Category="View")
	float MatchFinishViewBlendTimeDuration = 2.f;
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;

	UPROPERTY(Replicated)
	 FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* UIInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ShopToggleInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ToggleGameplayMenuAction;

	UFUNCTION()
	void ToggleShop();
	
	UFUNCTION()
    void ToggleGameplayMenu();
	void ShowWinLoseState();
};
