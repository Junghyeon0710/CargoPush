#include "ShopItemWidget.h"

#include "Crunch/Inventory/PA_ShopItem.h"


void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ShopItem = Cast<UPA_ShopItem>(ListItemObject);
	if (!ShopItem)
	{
		return;
	}

	SetIcon(ShopItem->GetIcon());
}
