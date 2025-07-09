// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CCharacter.generated.h"

class UWidgetComponent;
class UCAttributeSet;
class UCAbilitySystemComponent;

UCLASS()
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;
	//Only Server
	virtual void PossessedBy(AController* NewController) override;
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
	
protected:
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
};
