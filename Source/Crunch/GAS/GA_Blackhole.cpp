// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Blackhole.h"

#include "TargetActor_GroundPick.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "TA_BlackHole.h"

void UGA_BlackHole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	PlayCastBlackholeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargettingMontage);
	PlayCastBlackholeMontageTask->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackholeMontageTask->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackholeMontageTask->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackholeMontageTask->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackholeMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitPlacementTask->ValidData.AddDynamic(this, &UGA_BlackHole::PlaceBlackhole);
	WaitPlacementTask->Cancelled.AddDynamic(this, &UGA_BlackHole::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickTargetActor)
	{
		GroundPickTargetActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceRange(TargetTraceRange);
	}
	WaitPlacementTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UGA_BlackHole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAimEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BlackHole::PlaceBlackhole(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	RemoveAimEffect();

	if (PlayCastBlackholeMontageTask)
	{
		PlayCastBlackholeMontageTask->OnBlendOut.RemoveAll(this);
		PlayCastBlackholeMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlackholeMontageTask->OnInterrupted.RemoveAll(this);
		PlayCastBlackholeMontageTask->OnCompleted.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHoldBlackholeMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoldBlackholeMontage);
		PlayHoldBlackholeMontage->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayHoldBlackholeMontage->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayHoldBlackholeMontage->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayHoldBlackholeMontage->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayHoldBlackholeMontage->ReadyForActivation();
	}

	BlackholeTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackholeTargetActorClass);
	BlackholeTargetingTask->ValidData.AddDynamic(this, &UGA_BlackHole::FinalTargetsReceived);
	BlackholeTargetingTask->Cancelled.AddDynamic(this, &UGA_BlackHole::FinalTargetsReceived);
	BlackholeTargetingTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	BlackholeTargetingTask->BeginSpawningActor(this, BlackholeTargetActorClass, TargetActor);
	ATA_BlackHole* BlackholeTargetActor = Cast<ATA_BlackHole>(TargetActor);
	if (BlackholeTargetActor)
	{
		BlackholeTargetActor->ConfigureBlackhole(TargetAreaRadius, BlackholePullSpeed,BlackholeDuration, GetOwnerTeamId());
	}
	BlackholeTargetingTask->FinishSpawningActor(this, TargetActor);
	if (BlackholeTargetActor)
	{
		BlackholeTargetActor->SetActorLocation(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint);
	}
}

void UGA_BlackHole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
}

void UGA_BlackHole::FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, FinalBlowDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		FVector BlowCenter = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
		PushTargetsFromLocation(TargetDataHandle, BlowCenter,  BlowPushSpeed);

		UAbilityTask_PlayMontageAndWait* PlayFinalBlowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlowMontage);
		PlayFinalBlowMontage->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(FinalBlowMontage);
	}
}

void UGA_BlackHole::AddAimEffect()
{
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_BlackHole::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}