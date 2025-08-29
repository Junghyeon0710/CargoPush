// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/Character/PA_CharacterDefination.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CharacterDefination = Cast<UPA_CharacterDefination>(ListItemObject);
	if (CharacterDefination)
	{
		CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureMatParamName, CharacterDefination->LoadIcon());
		CharacterNameText->SetText(FText::FromString(CharacterDefination->GetCharacterDisplayName()));
	}
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected)
{
	CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, bIsSelected ? 0.f : 1.f);
}