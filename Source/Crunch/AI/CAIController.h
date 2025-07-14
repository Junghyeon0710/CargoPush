// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "CAIController.generated.h"

class UAISenseConfig_Sight;
class UAIPerceptionComponent;

UCLASS()
class CRUNCH_API ACAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACAIController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetBlackboardKeyName = "Target";
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	const UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

private:
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

	UFUNCTION()
	void TargetPerceptionForgotten(AActor* ForgottenActor);

	AActor* GetNextPerceivedActor() const;

	void ForgetActorIfDead(AActor* ActorToForget);
};
