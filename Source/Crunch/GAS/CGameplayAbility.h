// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

class UAnimInstance;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:

	UAnimInstance* GetOwnerAnimInstance() const;
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, bool bIgnoreSelf = true);
public:
};
