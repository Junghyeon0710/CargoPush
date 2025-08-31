// Fill out your copyright notice in the Description page of Project Settings.


#include "PA_CharacterDefination.h"

#include "CCharacter.h"

FPrimaryAssetId UPA_CharacterDefination::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinationAssetType(), GetFName());
}

FPrimaryAssetType UPA_CharacterDefination::GetCharacterDefinationAssetType()
{
	return FPrimaryAssetType(TEXT("CharacterDefination"));
}

UTexture2D* UPA_CharacterDefination::LoadIcon() const
{
	CharacterIcon.LoadSynchronous();
	if (CharacterIcon.IsValid())
	{
		return CharacterIcon.Get();
	}
	return nullptr;
}

TSubclassOf<ACCharacter> UPA_CharacterDefination::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid())
	{
		return CharacterClass.Get();
	}
	
	return TSubclassOf<ACCharacter>();
}


TSubclassOf<UAnimInstance> UPA_CharacterDefination::GetDisplayAnimBP() const
{
	DisplayAnimBP.LoadSynchronous();
	if (DisplayAnimBP.IsValid())
	{
		return DisplayAnimBP.Get();
	}
	return nullptr;
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* UPA_CharacterDefination::GetAbilities() const
{
	TSubclassOf<ACCharacter> LoadCharacter = LoadCharacterClass();
	if (!LoadCharacter)
	{
		return nullptr;
	}
	
	ACCharacter* Character = Cast<ACCharacter>(LoadCharacter.GetDefaultObject());
	if (!Character)
	{
		return nullptr;
	}

	return &Character->GetAbilities();
}

class USkeletalMesh* UPA_CharacterDefination::LoadDisplayMesh() const
{
	TSubclassOf<ACCharacter> LoadCharacter = LoadCharacterClass();
	if (!LoadCharacter)
	{
		return nullptr;
	}

	ACharacter* Character = Cast<ACharacter>(LoadCharacter.GetDefaultObject());
	if (!Character)
	{
		return nullptr;
	}

	return Character->GetMesh()->GetSkeletalMeshAsset();
}
