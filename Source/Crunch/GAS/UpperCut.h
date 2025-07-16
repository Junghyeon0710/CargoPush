// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbility.h"
#include "UpperCut.generated.h"

class UAnimMontage;

UCLASS()
class CRUNCH_API UUpperCut : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = GamepalyEffect)
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<UAnimMontage> UpperCutMontage;

	UPROPERTY(EditDefaultsOnly, Category = Targetting)
	float TargetSweepSphereRadius = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = Launch)
	float UpperCutLaunchSpeed = 1000.f;

	static FGameplayTag GetUpperCutLaunchTag();

	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);
};
