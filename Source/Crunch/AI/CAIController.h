// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "CAIController.generated.h"

class UAISenseConfig_Sight;
class UAIPerceptionComponent;

UCLASS()
class CRUNCH_API ACAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACAIController();

	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
