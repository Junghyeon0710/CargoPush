// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameAbilityTypes.h"
#include "CGameplayAbility.h"
#include "UpperCut.generated.h"

class UAnimMontage;

UCLASS()
class CRUNCH_API UUpperCut : public UCGameplayAbility
{
	GENERATED_BODY()
	UUpperCut();
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = GamepalyEffect)
	TMap<FName, FGenericDamageEffectDef> ComboDamageMap;
	
	UPROPERTY(EditDefaultsOnly, Category = GamepalyEffect)
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<UAnimMontage> UpperCutMontage;

	UPROPERTY(EditDefaultsOnly, Category = Targetting)
	float TargetSweepSphereRadius = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = Launch)
	float UpperCutLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = Launch)
	float UpperComboHoldSpeed = 100.f;

	static FGameplayTag GetUpperCutLaunchTag();
	const FGenericDamageEffectDef* GetDamageEffectDefForCurrentCombo() const;

	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData EventData);

	FName NextComboName;
};
