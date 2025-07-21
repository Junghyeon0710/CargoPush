// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameAbilityTypes.h"
#include "CGameplayAbility.h"
#include "GA_GroundBlast.generated.h"

class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_GroundBlast : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_GroundBlast();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float TargetAreaRadius =  300.f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FGenericDamageEffectDef DamageEffectDef;
	

	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	float TargetTraceRange = 2000.f;
		
	UPROPERTY(EditDefaultsOnly, Category = Targeting)
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> GroundBlastMontage;

	UFUNCTION()
	void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
