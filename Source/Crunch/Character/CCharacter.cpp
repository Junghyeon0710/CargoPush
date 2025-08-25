// Fill out your copyright notice in the Description page of Project Settings.


#include "CCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Crunch/Crunch.h"
#include "Crunch/GAS/CAbilitySystemComponent.h"
#include "Crunch/GAS/CAttributeSet.h"
#include "Crunch/GAS/UCAbilitySystemStatics.h"
#include "Crunch/Widgets/OverHeadStatsGauge.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	
	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("CAbilitySystem Component");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("Attributes");

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Over Head WidgetComponent");
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGASChangeDelegate();

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Perception Stimuli Source Component");
}

void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent->ServerSideInit();

}

void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

void ACCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACCharacter, TeamID);
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& ACCharacter::GetAbilities() const
{
	return CAbilitySystemComponent->GetAbilities();
}

FVector ACCharacter::GetCaptureLocalPosition() const
{
	return HeadshotCaptureLocalPosition;
}

FRotator ACCharacter::GetCaptureLocalRotation() const
{
	return HeadshotCaptureLocalRotation;
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatusWidget();
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
}

void ACCharacter::UpgradeAbilityWithInputID(ECAbilityInputID InputId)
{
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->Server_UpgradeAbilityWithID(InputId);
	}
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACCharacter::BindGASChangeDelegate()
{
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &ThisClass::DeathTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetStunStatTag()).AddUObject(this, &ThisClass::StunTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &ThisClass::AimTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetFocusStatTag()).AddUObject(this, &ThisClass::FocusTagUpdated);

		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::MoveSpeedUpdated);
		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthUpdated);
		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaUpdated);
	}
}

void ACCharacter::DeathTagUpdated(const FGameplayTag DeadTag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ACCharacter::StunTagUpdated(const FGameplayTag DeadTag, int32 NewCount)
{
	if (IsDead())
	{
		return;
	}

	if (NewCount != 0)
	{
		OnStun();
		PlayAnimMontage(StunMontage);
	}
	else
	{
		OnRecoverFromStun();
		StopAnimMontage();
	}
}

void ACCharacter::AimTagUpdated(const FGameplayTag DeadTag, int32 NewCount)
{
	SetIsAiming(NewCount != 0);
}

void ACCharacter::FocusTagUpdated(const FGameplayTag FocusTag, int32 NewCount)
{
	bIsInFocusMode = NewCount > 0;
}

void ACCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw = bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
	OnAimStateChanged(bIsAiming);
}

void ACCharacter::OnAimStateChanged(bool bIsAiming)
{
}

void ACCharacter::MoveSpeedUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ACCharacter::MaxHealthUpdated(const FOnAttributeChangeData& Data)
{
	if (IsValid(CAttributeSet))
	{
		CAttributeSet->RescaleHealth();
	}
	
}

void ACCharacter::MaxManaUpdated(const FOnAttributeChangeData& Data)
{
	if (IsValid(CAttributeSet))
	{
		CAttributeSet->RescaleMana();
	}
}

void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
}

bool ACCharacter::Server_SendGameplayEventToSelf_Validate(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	return true;
}

void ACCharacter::Server_SendGameplayEventToSelf_Implementation(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void ACCharacter::ConfigureOverHeadStatusWidget()
{
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(HeadStatGaugeVisibilityUpdateTimerHandle, this, &ThisClass::UpdateHeadGaugeVisibility, HeadStatGaugeVisibilityCheckUpdateGap, true);
	}
}

void ACCharacter::UpdateHeadGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}

void ACCharacter::SetStatusGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	
	if (bIsEnabled)
	{
		ConfigureOverHeadStatusWidget();
	}
	else
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

void ACCharacter::OnStun()
{
}

void ACCharacter::OnRecoverFromStun()
{
}

bool ACCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(UCAbilitySystemStatics::GetDeadStatTag());
}

void ACCharacter::RespawnImmediately()
{
	if (HasAuthority())
	{
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(UCAbilitySystemStatics::GetDeadStatTag()));
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &ThisClass::DeathMontageFinished, MontageDuration + DeathMontageFinishTimeShift);
	}
}

void ACCharacter::StartDeathSequence()
{
	OnDead();
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->CancelAllAbilities();
	}
	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);
//	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetAIPerceptionStimuliSourceEnabled(false);
}

void ACCharacter::Respawn()
{
	OnRespawn();
	SetAIPerceptionStimuliSourceEnabled(true);
	SetRagdollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->ApplyFullStatEffect();
	}
}

void ACCharacter::OnDead()
{
}

void ACCharacter::OnRespawn()
{
}

void ACCharacter::DeathMontageFinished()
{
	SetRagdollEnabled(true);
}

void ACCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void ACCharacter::OnRep_TeamID()
{
	
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent)
	{
		return;
	}

	if (bIsEnabled)
	{
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	}
	else
	{
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem();
	}
}

