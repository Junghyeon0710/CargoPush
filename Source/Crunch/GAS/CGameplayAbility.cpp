﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAP_Launched.h"
#include "UCAbilitySystemStatics.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"


UCGameplayAbility::UCGameplayAbility()
{
	ActivationBlockedTags.AddTag(UCAbilitySystemStatics::GetStunStatTag());
}

bool UCGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Level <= 0)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

AActor* UCGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
	AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (OwnerAvatarActor)
	{
		FVector Location;
		FRotator Rotation;
		OwnerAvatarActor->GetActorEyesViewPoint(Location, Rotation);
		
		FVector AimEnd = Location + Rotation.Vector() * AimDistance;

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(OwnerAvatarActor);
		
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		if (ShouldDrawDebug())
		{
			DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);
		}

		TArray<FHitResult> HitResults;
		if (GetWorld()->LineTraceMultiByObjectType(HitResults, Location, AimEnd, CollisionObjectQueryParams, CollisionQueryParams))
		{
			for (FHitResult& HitResult : HitResults)
			{
				if (IsActorTeamAttitudeIs(HitResult.GetActor(), TeamAttitude))
				{
					return HitResult.GetActor();
				}
			}
		}
	}

	return nullptr;
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	const USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeam, bool bDrawDebug, bool bIgnoreSelf)
{
	TArray<FHitResult> OutResults;
	TSet<AActor*> HitActors;

	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	
	for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
	{
		FVector StartLoc = TargetData->GetOrigin().GetTranslation();
		FVector EndLoc = TargetData->GetEndPoint();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> HitResults;
		
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StartLoc, EndLoc,
			SphereSweepRadius,
			ObjectTypes,
			false,
			ActorsToIgnore, DrawDebugTrace,
			HitResults,
			false
			);
		
		for (const FHitResult& HitResult : HitResults)
		{
			if (HitActors.Contains(HitResult.GetActor()))
			{
				continue;
			}

			if (OwnerTeamInterface)
			{
				ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor());
				if(OtherActorTeamAttitude != TargetTeam)
				{
					continue;
				}
			}

			HitActors.Add(HitResult.GetActor());
			OutResults.Add(HitResult);
		}
	}
	return OutResults;
}

void UCGameplayAbility::PushSelf(const FVector& PushVel)
{
	ACharacter* OwningAvatarCharacter = GetOwningAvatarCharacter();
	if (OwningAvatarCharacter)
	{
		OwningAvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel)
{
	if (!Target)
	{
		return;
	}

	FGameplayEventData EventData;

	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVel;
	HitData->HitResult = HitResult;
	EventData.TargetData.Add(HitData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, UGAP_Launched::GetLaunchAbilityActivationTag(), EventData);
}

void UCGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVal)
{
	for (AActor* TargetActor : Targets)
	{
		PushTarget(TargetActor, PushVal);
	}
}

void UCGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVal)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargets(TargetActors, PushVal);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed)
{
	AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (!OwnerAvatarActor)
		return;

	FVector OwnerAvatarActorLocation = OwnerAvatarActor->GetActorLocation();
	PushTargetsFromLocation(Targets, OwnerAvatarActorLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromOwnerLocation(TargetActors, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& FromLocation, float PushSpeed)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromLocation(Targets, FromLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed)
{
	for (AActor* Target : Targets)
	{
		FVector PushDir = Target->GetActorLocation() - FromLocation;
		PushDir.Z = 0;
		PushDir.Normalize();

		PushTarget(Target, PushDir * PushSpeed);
	}
}

void UCGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst && !OwnerAnimInst->Montage_IsPlaying(MontageToPlay))
	{
		OwnerAnimInst->Montage_Play(MontageToPlay);
	}
}

void UCGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst)
	{
		FName CurrentSectionName = OwnerAnimInst->Montage_GetCurrentSection(MontageToStop);
		OwnerAnimInst->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
	}
}

FGenericTeamId UCGameplayAbility::GetOwnerTeamId() const
{
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamInterface)
	{
		return OwnerTeamInterface->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

bool UCGameplayAbility::IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
	if (!OtherActor)
		return false;

	IGenericTeamAgentInterface* OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamAgentInterface)
	{
		return OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
	}

	return false;
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
	{
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}

	return AvatarCharacter;
}

void UCGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC)
	{
		OwnerASC->HandleGameplayEvent(EventTag, &EventData);
	}
}
