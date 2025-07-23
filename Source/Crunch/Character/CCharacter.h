// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Crunch/GAS/CGameAbilityTypes.h"
#include "CCharacter.generated.h"

struct FOnAttributeChangeData;
class UWidgetComponent;
class UCAttributeSet;
class UCAbilitySystemComponent;
struct FGameplayEventData;
class UGameplayAbility;

UCLASS()
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;
	//Only Server
	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/********************************************************/
	/*					/** Gameplay Ability *		        */
	/********************************************************/
	
	/** Gameplay Ability */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* ~Gameplay Ability **/

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	
protected:
	void BindGASChangeDelegate();
	void DeathTagUpdated(const FGameplayTag DeadTag, int32 NewCount);
	void StunTagUpdated(const FGameplayTag DeadTag, int32 NewCount);
	void AimTagUpdated(const FGameplayTag DeadTag, int32 NewCount);
	void SetIsAiming(bool bIsAiming);
	virtual void OnAimStateChanged(bool bIsAiming);
	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);
	void MaxHealthUpdated(const FOnAttributeChangeData& Data);
	void MaxManaUpdated(const FOnAttributeChangeData& Data);
	
	UPROPERTY(visibleDefaultsOnly, Category="Gameplay Ability")
	TObjectPtr<UCAbilitySystemComponent> CAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCAttributeSet> CAttributeSet;

	/********************************************************/
	/*					/** UI *							*/
	/********************************************************/

private:
	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	TObjectPtr<UWidgetComponent> OverHeadWidgetComponent;

	void ConfigureOverHeadStatusWidget();

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatGaugeVisibilityCheckUpdateGap = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatGaugeVisibilityRangeSquared = 10000000.0f;
	
	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;
	void UpdateHeadGaugeVisibility();
	void SetStatusGaugeEnabled(bool bIsEnabled);
	/*******************************************************/
	/*                 Stun								   */
	/*******************************************************/

private:

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TObjectPtr<UAnimMontage> StunMontage;

	virtual void OnStun();
	virtual void OnRecoverFromStun();
		
	/*******************************************************/
	/*                 Death and Respawn				   */
	/*******************************************************/
public:
	bool IsDead() const;
	void RespawnImmediately();
private:
	FTransform MeshRelativeTransform;
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TObjectPtr<UAnimMontage> DeathMontage;

	void PlayDeathAnimation();
	
	void StartDeathSequence();
	void Respawn();

	virtual void OnDead();
	virtual void OnRespawn();
	
	FTimerHandle DeathMontageTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimeShift = -0.8f;
	
	void DeathMontageFinished();
	void SetRagdollEnabled(bool bIsEnabled);

	/*******************************************************/
	/*                Team								   */
	/*******************************************************/

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();

	/*******************************************************/
	/*                AI								   */
	/*******************************************************/
private:
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled);
	
	UPROPERTY()
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> PerceptionStimuliSourceComponent;
};
