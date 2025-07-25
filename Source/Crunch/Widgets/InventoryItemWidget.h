﻿#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
/**
 * 
 */
UCLASS()
class UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	bool IsEmpty() const;
	void UpdateInventoryItem(const UInventoryItem* Item);
	void EmptySlot();
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }

private:
	void UpdateStackCount();
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UTexture2D* EmptyTexture;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* StackCountText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCountText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ManaCostText;

	UPROPERTY()
	const UInventoryItem* InventoryItem;

	int SlotNumber;
};