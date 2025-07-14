// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionBarrack.h"

#include "Minion.h"
#include "GameFramework/PlayerStart.h"


AMinionBarrack::AMinionBarrack()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();
	SpawnNewMinions(5);
}

void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0)
	{
		return nullptr;
	}

	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0;
	}

	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnNewMinions(int Amount)
{
	for (int i = 0; i < Amount; ++i)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSPot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSPot->GetActorTransform();
		}

		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		NewMinion->SetGenericTeamId(BarrackTeamId);
		NewMinion->FinishSpawning(SpawnTransform);
		MinionPool.Add(NewMinion);
	}
}

