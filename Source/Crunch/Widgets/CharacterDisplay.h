// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"
class UPA_CharacterDefination;

UCLASS()
class CRUNCH_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterDisplay();
	void ConfigureWithCharacterDefination(const UPA_CharacterDefination* CharacterDefination);
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	class UCameraComponent* CameraComponent;
};
