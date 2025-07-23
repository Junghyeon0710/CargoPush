// Fill out your copyright notice in the Description page of Project Settings.


#include "GAP_Dead.h"

#include "AbilitySystemComponent.h"
#include "CHeroAttributeSet.h"
#include "UCAbilitySystemStatics.h"
#include "Engine/OverlapResult.h"

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
		if (!Killer || UCAbilitySystemStatics::IsHero(Killer))
		{
			Killer = nullptr;
		}

		TArray<AActor*> RewardTargets = GetRewardTargets();
		if (RewardTargets.Num() == 0 && !Killer)
		{
			K2_EndAbility();
			return;
		}

		if (Killer && !RewardTargets.Contains(Killer))
		{
			RewardTargets.Add(Killer);
		}

		bool bFound = false;
		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);

		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
		float TotalGoldReward = BaseExperienceReward + GoldRewardPerExperience * SelfExperience;

		
		
	}
}

TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	TSet<AActor*> OutActors;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		return OutActors.Array();
	}

	FCollisionObjectQueryParams CollisionObjectParams;
	CollisionObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);
	
	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, CollisionObjectParams, CollisionShape))
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OverlapResult.GetActor());
			if (!OtherTeamInterface || OtherTeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
			{
				continue;
			}

			if (!UCAbilitySystemStatics::IsHero(OverlapResult.GetActor()))
			{
				continue;
			}

			OutActors.Add(OverlapResult.GetActor());
		}
	}

	return OutActors.Array();
}
