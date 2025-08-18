// Fill out your copyright notice in the Description page of Project Settings.


#include "CAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Crunch/GAS/UCAbilitySystemStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &ThisClass::OwnerAimTagChanged);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();
		FRotator BodyRot = OwnerCharacter->GetActorRotation();
		FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, BodyPrevRot);
		BodyPrevRot = BodyRot;

		YawSpeed = BodyRotDelta.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerpSpeed);
		FRotator ControllerRot = OwnerCharacter->GetBaseAimRotation();
		LookRotOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControllerRot, BodyRot);

		FwdSpeed = Velocity.Dot(ControllerRot.Vector());
		RightSpeed = -Velocity.Dot(ControllerRot.Vector().Cross(FVector::UpVector));
	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
	}
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

bool UCAnimInstance::ShouldDoFullbody() const
{
	return GetSpeed() <= 0 && !GetIsAiming();
}

void UCAnimInstance::OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount != 0;
}
