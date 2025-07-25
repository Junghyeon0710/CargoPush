﻿#include "ItemToolTip.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/Inventory/PA_ShopItem.h"

void UItemToolTip::SetItem(const UPA_ShopItem* Item)
{
	ItemTitleText->SetText(Item->GetItemName());
	ItemDescriptionText->SetText(Item->GetItemDescription());
	ItemPriceText->SetText(FText::AsNumber((int)Item->GetPrice()));
	IconImage->SetBrushFromTexture(Item->GetIcon());
}

void UItemToolTip::SetPrice(float NewPrice)
{
	ItemPriceText->SetText(FText::AsNumber((int)NewPrice));
}