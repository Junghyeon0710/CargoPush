
#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
class UShopItemWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssused, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);


UCLASS()
class CRUNCH_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }

	FOnItemPurchaseIssused OnItemPurchaseIssued;
	FOnShopItemSelected OnShopItemClicked;
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;
};