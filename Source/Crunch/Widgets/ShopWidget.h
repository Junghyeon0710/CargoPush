#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UPA_ShopItem;
class UTileView;
class UShopItemWidget;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class UShopWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTileView> ShopItemList;

	void LoadShopItems();
	void ShopItemLoadFinished();
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);

	UPROPERTY()
	TMap<const UPA_ShopItem*, const UShopItemWidget*> ItemsMap;

	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;
};