// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetActor_GroundPick.h"


// Sets default values
ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATargetActor_GroundPick::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetActor_GroundPick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

