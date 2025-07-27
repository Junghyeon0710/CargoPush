#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crunch/Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryItemWidget;

UCLASS()
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta=(BindWidget))
	class UWrapBox* ItemList;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

	UPROPERTY()
	class UInventoryComponent* InventoryComponent;

	TArray<UInventoryItemWidget*> ItemWidgets;
	TMap<FInventoryItemHandle, UInventoryItemWidget*>  PopulatedItemEntryWidgets;

	void ItemAdded(const UInventoryItem* InventoryItem);
	void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

	UInventoryItemWidget* GetNextAvaliableSlot() const;
};