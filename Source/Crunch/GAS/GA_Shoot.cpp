// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Shoot.h"

#include "GameplayTagsManager.h"
#include "ProjectileActor.h"
#include "UCAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

class UAbilityTask_WaitGameplayEvent;

UGA_Shoot::UGA_Shoot()
{
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetAimStatTag());
	
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetCrosshairTag());
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Activated"));

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UCAbilitySystemStatics::GetBasicAttackInputPressedTag());
		WaitStartShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StartShooting);
		WaitStartShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UCAbilitySystemStatics::GetBasicAttackInputReleasedTag());
		WaitStopShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StopShooting);
		WaitStopShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootTag(), nullptr, false, false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &UGA_Shoot::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Ended"));
	K2_EndAbility();
}

FGameplayTag UGA_Shoot::GetShootTag()
{
	return FGameplayTag::RequestGameplayTag("ability.shoot");
}

void UGA_Shoot::StartShooting(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Start Shooting"));

	// UAbilityTask_NetworkSyncPoint* NetworkSync = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait);
	// NetworkSync->ReadyForActivation();
	//
	// UAbilityTask_PlayMontageAndWait* PlayShootMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
	// PlayShootMontage->ReadyForActivation();
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayShootMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		PlayShootMontage->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(ShootMontage);
	}
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Shooting"));
	if (ShootMontage)
	{
		StopMontageAfterCurrentSection(ShootMontage);
	}
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot Projectile"));
	if (K2_HasAuthority())
	{
		AActor* OwnerAvaterActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerAvaterActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo();
		if (MeshComp)
		{
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num() != 0)
			{
				FName SocketName = OutNames.Last();
				SocketLocation = MeshComp->GetSocketLocation(SocketName);
			}
		}

		AProjectileActor* Projectile = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SocketLocation, OwnerAvaterActor->GetActorRotation(), SpawnParams);
		if (Projectile)
		{
			Projectile->ShootProjectile(ShootProjectileSpeed, ShootProjectileRange, GetAimTargetIfValid(), GetOwnerTeamId(), MakeOutgoingGameplayEffectSpec(ProjectileHitEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)));
		}
	}
}

AActor* UGA_Shoot::GetAimTargetIfValid() const
{
	AActor* AimTarget = GetAimTarget(ShootProjectileRange, ETeamAttitude::Hostile);
	return AimTarget;
}
