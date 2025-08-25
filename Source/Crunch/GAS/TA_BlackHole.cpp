// Fill out your copyright notice in the Description page of Project Settings.


#include "TA_BlackHole.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

ATA_BlackHole::ATA_BlackHole()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>("Detection Sphere Component");
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	DetectionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATA_BlackHole::ActorInBlackholeRange);
	DetectionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ATA_BlackHole::ActorLeftBlackholeRange);

	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
	PrimaryActorTick.bCanEverTick = true;

	VFXComponent = CreateDefaultSubobject<UParticleSystemComponent>("VFX Component");
	VFXComponent->SetupAttachment(GetRootComponent());
}

void ATA_BlackHole::ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId)
{
	PullSpeed = InPullSpeed;
	DetectionSphereComponent->SetSphereRadius(InBlackholeRange);
	SetGenericTeamId(InTeamId);
	BlackholeDuration = InBlackholeDuration;
	BlackholeRange = InBlackholeRange;
}

void ATA_BlackHole::SetGenericTeamId(const FGenericTeamId& InTeamId)
{
	TeamId = InTeamId;
}

void ATA_BlackHole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATA_BlackHole, TeamId);
	DOREPLIFETIME_CONDITION_NOTIFY(ATA_BlackHole, BlackholeRange, COND_None, REPNOTIFY_Always);
}

void ATA_BlackHole::StartTargeting(class UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(BlackholeDurationTimerHandle, this, &ATA_BlackHole::StopBlackhole, BlackholeDuration);
	}
}

void ATA_BlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
    	{
    		for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
    		{
    			AActor* Target = TargetPair.Key;
    			UNiagaraComponent* NiagaraComponent = TargetPair.Value;
    
    			FVector PullDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
    			Target->SetActorLocation(Target->GetActorLocation() + PullDir * PullSpeed * DeltaTime);
    
    			if (NiagaraComponent)
    			{
    				NiagaraComponent->SetVariablePosition(BlackholeVFXOriginVariableName, VFXComponent->GetComponentLocation());
    			}
    		}
    	}
}

void ATA_BlackHole::ConfirmTargetingAndContinue()
{
	StopBlackhole();
}

void ATA_BlackHole::CancelTargeting()
{
	StopBlackhole();
	Super::CancelTargeting();
}

void ATA_BlackHole::TryAddTarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget))
		return;

	if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile)
		return;

	UNiagaraComponent* NiagaraComponent = nullptr;
	if (BlackholeLinkVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BlackholeLinkVFX, OtherTarget->GetRootComponent(), NAME_None, FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariablePosition(BlackholeVFXOriginVariableName, VFXComponent->GetComponentLocation());
		}
	}

	ActorsInRangeMap.Add(OtherTarget, NiagaraComponent);
}

void ATA_BlackHole::RemoveTarget(AActor* OtherTarget)
{
	if (!OtherTarget)
	{
		return;
	}

	if (ActorsInRangeMap.Contains(OtherTarget))
	{
		UNiagaraComponent* VFXComp;
		ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VFXComp);
		if(IsValid(VFXComp))
		{
			VFXComp->DestroyComponent();
		}
	}
}

void ATA_BlackHole::StopBlackhole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
	{
		FinalTargets.Add(TargetPair.Key);
		UNiagaraComponent* NiagaraComponent = TargetPair.Value;
		if (IsValid(NiagaraComponent))
		{
			NiagaraComponent->DestroyComponent();
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	FGameplayAbilityTargetData_ActorArray* TargetActorArray = new FGameplayAbilityTargetData_ActorArray;
	TargetActorArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetActorArray);

	FGameplayAbilityTargetData_SingleTargetHit* BlowupLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	BlowupLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(BlowupLocation);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATA_BlackHole::OnRep_BlackholeRange()
{
	DetectionSphereComponent->SetSphereRadius(BlackholeRange);
}

void ATA_BlackHole::ActorInBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryAddTarget(OtherActor);
}

void ATA_BlackHole::ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveTarget(OtherActor);
}