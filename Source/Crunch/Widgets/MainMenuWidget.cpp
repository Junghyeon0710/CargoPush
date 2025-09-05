// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "SessionEntryWidget.h"
#include "WaitingWidget.h"
#include "Crunch/Framework/CGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Crunch/Network/CNetStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		CGameInstance->OnLoginCompleted.AddUObject(this, &UMainMenuWidget::LoginCompleted);
		if (CGameInstance->IsLoggedIn())
		{
			SwitchToMainWidget();
		}

		CGameInstance->OnJoinSessionFailed.AddUObject(this, &UMainMenuWidget::JoinSessionFailed);
		CGameInstance->OnGlobalSessionSearchCompleted.AddUObject(this, &UMainMenuWidget::UpdateLobbyList);
		CGameInstance->StartGlobalSessionSearch();
	}

	LoginBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginBtnClicked);
	
	CreateSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::CreateSessionBtnClicked);
	CreateSessionBtn->SetIsEnabled(false);

	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMainMenuWidget::NewSessionNameTextChanged);
	JoinSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::JoinSessionBtnClicked);
	JoinSessionBtn->SetIsEnabled(false);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
}

void UMainMenuWidget::CreateSessionBtnClicked()
{
	if (CGameInstance && CGameInstance->IsLoggedIn())
	{
		CGameInstance->RequestCreateAndJoinSession(FName(NewSessionNameText->GetText().ToString()));
		SwitchToWaitingWidget(FText::FromString("Creating Lobby"), true).AddDynamic(this, &UMainMenuWidget::CancelSessionCreation);
	}
}

void UMainMenuWidget::CancelSessionCreation()
{
	if (CGameInstance)
	{
		CGameInstance->CancelSessionCreation();
	}
	SwitchToMainWidget();
}

void UMainMenuWidget::NewSessionNameTextChanged(const FText& NewText)
{
	CreateSessionBtn->SetIsEnabled(!NewText.IsEmpty());
}

void UMainMenuWidget::JoinSessionFailed()
{
	SwitchToMainWidget();
}

void UMainMenuWidget::UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateLobbyList"))
	SessionScrollBox->ClearChildren();

	bool bCurrentSelectedSessionValid = false;
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		USessionEntryWidget* NewSession = CreateWidget<USessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (NewSession)
		{
			FString SessionName = "Name_None";
			SearchResult.Session.SessionSettings.Get<FString>(UCNetStatics::GetSessionNameKey(), SessionName);

			FString SessionIdStr = SearchResult.Session.GetSessionIdStr();
			NewSession->InitializeEntry(SessionName, SessionIdStr);
			NewSession->OnSessionEntrySelected.AddUObject(this, &UMainMenuWidget::SessionEntrySelected);
			SessionScrollBox->AddChild(NewSession);
			if (CurrentSelectedSessionId == SessionIdStr)
			{
				bCurrentSelectedSessionValid = true;
			}
		}
	}

	CurrentSelectedSessionId = bCurrentSelectedSessionValid ? CurrentSelectedSessionId : "";
	JoinSessionBtn->SetIsEnabled(bCurrentSelectedSessionValid);
}

void UMainMenuWidget::JoinSessionBtnClicked()
{
	if (CGameInstance && !CurrentSelectedSessionId.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining Session id :%s"), *CurrentSelectedSessionId);
		CGame
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Session Selected"));
	}
}

void UMainMenuWidget::SessionEntrySelected(const FString& SelectedEntryIdStr)
{
	CurrentSelectedSessionId = SelectedEntryIdStr;
}

void UMainMenuWidget::LoginBtnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Logining In!"))
	if (CGameInstance && !CGameInstance->IsLoggedIn() && !CGameInstance->IsLoggingIn())
	{
		CGameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString("Logging In"));
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login successful"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Failed!"))
	}

	SwitchToMainWidget();
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitInfo(WaitInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}
