// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

UCLASS()
class CRUNCH_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterDisplay();
	void ConfigureWithCharacterDefination(class UPA_CharacterDefination* CharacterDefination);
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	class UCameraComponent* CameraComponent;
};
