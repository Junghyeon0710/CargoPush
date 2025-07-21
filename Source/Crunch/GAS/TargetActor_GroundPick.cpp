// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetActor_GroundPick.h"

#include "Crunch/Crunch.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetActor_GroundPick::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATargetActor_GroundPick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		SetActorLocation(GetTargetPoint());
	}
		
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController())
	{
		return GetActorLocation();
	}

	FHitResult TraceResult;

	FVector ViewLocation;
	FRotator ViewRot;

	PrimaryPC->GetPlayerViewPoint(ViewLocation, ViewRot);

	FVector TraceEnd = ViewLocation + ViewRot.Vector() * TargetTraceRange;
	
	GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd, ECC_Target);

	if (!TraceResult.bBlockingHit)
	{
		GetWorld()->LineTraceSingleByChannel(TraceResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Target);
	}

	if (!TraceResult.bBlockingHit)
	{
		return GetActorLocation();
	}

	return TraceResult.ImpactPoint;
}


