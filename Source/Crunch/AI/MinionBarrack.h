// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "MinionBarrack.generated.h"

class AMinion;
class APlayerStart;

UCLASS()
class CRUNCH_API AMinionBarrack : public AActor
{
	GENERATED_BODY()

public:
	AMinionBarrack();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId BarrackTeamId;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int MinionPerGroup = 3;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float GroupSpawnInterval = 5.f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TObjectPtr<AActor> Goal;
	
	UPROPERTY()
	TArray<AMinion*> MinionPool;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinion> MinionClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart*> SpawnSpots;

	int32 NextSpawnSpotIndex = -1;

	const APlayerStart* GetNextSpawnSpot();

	void SpawnNewGroup();
	void SpawnNewMinions(int Amount);
	AMinion* GetNextAvaliableMinion() const;

	FTimerHandle SpawnIntervalTimerHandle;
};
