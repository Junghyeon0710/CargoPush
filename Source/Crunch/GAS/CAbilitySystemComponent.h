// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CGameAbilityTypes.h"
#include "CAbilitySystemComponent.generated.h"

class UPA_AbilitySystemGenerics;
struct FOnAttributeChangeData;

UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UCAbilitySystemComponent();
	void InitializeBaseAttributes();
	void ServerSideInit();
	void ApplyFullStatEffect();
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

	bool IsAtMaxLevel() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithID(ECAbilityInputID InInputID);

	UFUNCTION(Client, Reliable)
	void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int NewLevel);
	
private:
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData);



	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TObjectPtr<UPA_AbilitySystemGenerics> AbilitySystemGenerics;
};
