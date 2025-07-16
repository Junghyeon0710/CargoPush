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
public:
	
protected:

	UAnimInstance* GetOwnerAnimInstance() const;
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, bool bIgnoreSelf = true);

	UFUNCTION()
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }
	void PushSelf(const FVector& PushVel);
	void PushTarget(AActor* Target, const FVector& PushVel);

	ACharacter* GetOwningAvatarCharacter();
private:
	UPROPERTY(EditDefaultsOnly, Category = Debug)
	bool bShouldDrawDebug = false;

	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
};
