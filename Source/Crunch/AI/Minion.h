﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crunch/Character/CCharacter.h"
#include "Minion.generated.h"

UCLASS()
class CRUNCH_API AMinion : public ACCharacter
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	bool IsActive() const;
	void Activate();
	void SetGoal(AActor* Goal);

private:
	void PicSkinBasedOnTeamID();

	virtual void OnRep_TeamID() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	FName GoalBlackboardKeyName = "Goal";
};
