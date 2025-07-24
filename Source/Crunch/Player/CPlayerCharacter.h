// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crunch/Character/CCharacter.h"
#include "GameplayTagContainer.h"
#include "CPlayerCharacter.generated.h"

class UCHeroAttributeSet;
enum class ECAbilityInputID : uint8;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()

public:
	ACPlayerCharacter();
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	UPROPERTY()
	TObjectPtr<UCHeroAttributeSet> HeroAttributeSet;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class UCameraComponent* ViewCamera;

	FVector GetLookRightDir() const;
	FVector GetLookFwdDir() const;
	FVector GetMoveFwdDir() const;

	/*******************************************************/
	/*                     Gameplay Ability				   */
	/*******************************************************/

private:
	virtual void OnAimStateChanged(bool bIsAiming) override;

	/*******************************************************/
	/*                     Input						   */
	/*******************************************************/

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* DefaultContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LearnAbilityLeaderAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<ECAbilityInputID, TObjectPtr<UInputAction>> GameplayAbilityInputActions;
	
private:

	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);
	void LearnAbilityLeaderDown(const FInputActionValue& InputActionValue);
	void LearnAbilityLeaderUp(const FInputActionValue& InputActionValue);
	bool bIsLearnAbilityLeaderDown = false;
	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID);
	void SetInputEnabledFromPlayerController(bool bEnabled);

	/*******************************************************/
	/*						Stun						   */
	/*******************************************************/
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;
	
	
	/*******************************************************/
	/*                 Death and Respawn				   */
	/*******************************************************/

	virtual void OnDead() override;
	virtual void OnRespawn() override;

	/*******************************************************/
	/*                 Camera View						   */
	/*******************************************************/

private:
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector CameraAimLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	float CameraLerpSpeed = 20.f;

	FTimerHandle CameraLerpTimerHandle;

	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLocalOffestLerp(FVector Goal);
};
