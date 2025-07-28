
#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "TreeNodeInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
class UShopItemWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssused, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);


UCLASS()
class CRUNCH_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }

	FOnItemPurchaseIssused OnItemPurchaseIssued;
	FOnShopItemSelected OnShopItemClicked;

	virtual UUserWidget* GetWidget() const override;
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	virtual TArray<const ITreeNodeInterface*> GetOuputs() const override;
	virtual const UObject* GetItemObject() const override;
private:
	void CopyFromOther(const UShopItemWidget* OtherWidget);
	void InitWithShopItem(const UPA_ShopItem* NewShopItem);
	TArray<const ITreeNodeInterface*> ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const;
	
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	const class UListView* ParentListView;
	
	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;
};