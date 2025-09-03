// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:	
	virtual void NativeConstruct() override;

	
	
	/******************************/	
	/*           Main             */
	/******************************/	
private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY()
	class UCGameInstance* CGameInstance;

	/******************************/	
	/*           Main             */
	/******************************/	
private:
	UPROPERTY(meta=(BindWidget))
	class UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	class UButton* LoginBtn;

	UFUNCTION()
	void LoginBtnClicked();

	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg);
};
