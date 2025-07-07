// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"

#include "CPlayerCharacter.h"

void ACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(InPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
	}
}

void ACPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	CPlayerCharacter = Cast<ACPlayerCharacter>(P);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
	}
}
