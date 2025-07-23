// Fill out your copyright notice in the Description page of Project Settings.


#include "GAP_Dead.h"

#include "UCAbilitySystemStatics.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = UCAbilitySystemStatics::GetDeadStatTag();
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.RemoveTag(UCAbilitySystemStatics::GetStunStatTag());
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		if (Killer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Killer : %s"), *Killer->GetName())
		}
	}
}
