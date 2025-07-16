// Fill out your copyright notice in the Description page of Project Settings.


#include "UpperCut.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UUpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!K2_CommitAbility())
	{
		K2_EndAbility()
		RETURN_IF_EXIT_REQUESTED;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayUpperCutMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayUpperCutMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayUpperCutMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayUpperCutMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayUpperCutMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayUpperCutMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLaunchEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetUpperCutLaunchTag());
		WaitLaunchEventTask->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
		WaitLaunchEventTask->ReadyForActivation();
	}
}

FGameplayTag UUpperCut::GetUpperCutLaunchTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.UpperCut.Launch");
}

void UUpperCut::StartLaunching(FGameplayEventData EventData)
{
	TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug());
	if (K2_HasAuthority())
	{
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperCutLaunchSpeed);
		for (FHitResult HitResult : TargetHitResults)
		{
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperCutLaunchSpeed);
		}
	}
}