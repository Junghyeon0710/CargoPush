// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
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

	void SwitchToMainWidget();

	UPROPERTY(meta=(BindWidget))
	class UWidget* MainWidgetRoot;

	/******************************/	
	/*           Session           */
	/******************************/

	UPROPERTY(meta=(BindWidget))
	class UButton* CreateSessionBtn;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* NewSessionNameText;

	UFUNCTION()
	void CreateSessionBtnClicked();

	UFUNCTION()
	void CancelSessionCreation();
	
	UFUNCTION()
	void NewSessionNameTextChanged(const FText& NewText);

	/******************************/	
	/*           Login             */
	/******************************/	
private:
	UPROPERTY(meta=(BindWidget))
	class UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	class UButton* LoginBtn;

	UFUNCTION()
	void LoginBtnClicked();

	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg);

	/******************************/	
	/*           Waiting          */
	/******************************/	
private:
	UPROPERTY(meta=(BindWidget))
	class UWaitingWidget* WaitingWidget;
	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel = false);
};
