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
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnIntervalTimerHandle,this, &ThisClass::SpawnNewGroup, GroupSpawnInterval, true);
	}
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

void AMinionBarrack::SpawnNewGroup()
{
	int i = MinionPerGroup;

	while (i > 0)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSPot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSPot->GetActorTransform();
		}

		AMinion* NextAvaliableMinion  = GetNextAvaliableMinion();

		if (!NextAvaliableMinion)
		{
			break; //풀에서 사용 가능 미니언이 없으면 빠져나옵니다.
		}

		NextAvaliableMinion->SetActorTransform(SpawnTransform);
		NextAvaliableMinion->Activate();
		--i;
	}

	// 없는 만큼 풀에 생성
	SpawnNewMinions(i);
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
		NewMinion->SetGoal(Goal);
		MinionPool.Add(NewMinion);
	}
}

AMinion* AMinionBarrack::GetNextAvaliableMinion() const
{
	for (AMinion* Minion : MinionPool)
	{
		if (!Minion->IsActive())
		{
			return Minion;
		}
	}

	return nullptr;
}

