// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterDisplay.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Crunch/Character/PA_CharacterDefination.h"

// Sets default values
ACharacterDisplay::ACharacterDisplay()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Comp"));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Comp");
	MeshComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("View Camera Component");
	CameraComponent->SetupAttachment(RootComponent);

}

void ACharacterDisplay::ConfigureWithCharacterDefination(class UPA_CharacterDefination* CharacterDefination)
{
	if (!CharacterDefination)
		return;

	MeshComponent->SetSkeletalMesh(CharacterDefination->LoadDisplayMesh());
	MeshComponent->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	MeshComponent->SetAnimClass(CharacterDefination->GetDisplayAnimBP());
}



