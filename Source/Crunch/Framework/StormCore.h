// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

UCLASS()
class CRUNCH_API AStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	AStormCore();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float MaxMoveSpeed = 500.f;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class USphereComponent* InfluenceRange;

	UFUNCTION()
	void NewInfluenerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateTeamWeight();
	void UpdateGoal();

	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamOneGoal;

	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamTwoGoal;

	int TeamOneInfluncerCount = 0;
	int TeamTwoInfluncerCount = 0;

	float TeamWeight = 0.f;

	UPROPERTY()
	class AAIController* OwnerAIC;
};
