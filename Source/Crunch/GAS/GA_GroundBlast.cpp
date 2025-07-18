// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_GroundBlast.h"

#include "UCAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetAimStatTag());
}

void UGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayGroundBlastAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GroundBlastMontage);
	PlayGroundBlastAnimTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayGroundBlastAnimTask->ReadyForActivation();
}
