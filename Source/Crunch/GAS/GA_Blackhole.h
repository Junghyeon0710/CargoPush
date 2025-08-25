// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbility.h"
#include "GA_Blackhole.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_BlackHole : public UCGameplayAbility
{
	GENERATED_BODY()
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	float TargetAreaRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackholePullSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackholeDuration = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TargettingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HoldBlackholeMontage;


	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> AimEffect;

	FActiveGameplayEffectHandle AimEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATA_BlackHole> BlackholeTargetActorClass;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* PlayCastBlackholeMontageTask;

	UPROPERTY()
	class UAbilityTask_WaitTargetData* BlackholeTargetingTask;

	UFUNCTION()
	void PlaceBlackhole(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void AddAimEffect();
	void RemoveAimEffect();
};
