// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"
#include "Crunch/Widgets/GameplayWidget.h"
#include "CPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

void ACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(InPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
		CPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void ACPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	CPlayerCharacter = Cast<ACPlayerCharacter>(P);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerController, TeamID)
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
		GameplayWidget->ConfigureAbilities(CPlayerCharacter->GetAbilities());
	}
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSubsystem)
	{
		InputSubsystem->RemoveMappingContext(UIInputMapping);
		InputSubsystem->AddMappingContext(UIInputMapping, 1);
	}

	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComp)
	{
		EnhancedInputComp->BindAction(ShopToggleInputAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleShop);
		EnhancedInputComp->BindAction(ToggleGameplayMenuAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleGameplayMenu);
	}
}

void ACPlayerController::ToggleShop()
{
	if(GameplayWidget)
	{
		GameplayWidget->ToggleShop();
	}
}

void ACPlayerController::ToggleGameplayMenu()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleGameplayMenu();
	}
}