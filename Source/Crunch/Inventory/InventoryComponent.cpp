﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "PA_ShopItem.h"
#include "Crunch/Framework/CAssetManager.h"
#include "Crunch/GAS/CHeroAttributeSet.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	
}

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem)
		return;

	InventoryItem->TryActivateGrantedAbility(OwnerAbilitySystemComponent);
	const UPA_ShopItem* Item = InventoryItem->GetShopItem();
	if (Item->GetIsConsumable())
	{
		ConsumeItem(InventoryItem);
	}
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem)
		return;

	Server_ActivateItem(ItemHandle);
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase)
		return;

	if (GetGold() < ItemToPurchase->GetPrice())
		return;

	if (IsFullFor(ItemToPurchase))
		return;

	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	GrantItem(ItemToPurchase);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}


void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent)
		return;

	Server_Purchase(ItemToPurchase);
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& ItemHandle)
{
	Server_SellItem(ItemHandle);
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem || !InventoryItem->IsValid())
		return;

	float SellPrice = InventoryItem->GetShopItem()->GetSellPrice();
	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice * InventoryItem->GetStackCount());
	RemoveItem(InventoryItem);
}
bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

float UInventoryComponent::GetGold() const
{
	bool bFound = false;
	if (OwnerAbilitySystemComponent)
	{
		float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
		{
			return Gold;
		}
	}

	return 0.f;
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (UInventoryItem* StackItem = GetAvaliableStackForItem(NewItem))
	{
		StackItem->AddStackCount();
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
		Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
	}
	else
	{
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		InventoryItem->InitItem(NewHandle, NewItem);
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);
		UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
		Client_ItemAdded(NewHandle, NewItem);
		InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent);
		CheckItemCombination(InventoryItem);
	}
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (!Item)
		return;

	Item->ApplyConsumeEffect(OwnerAbilitySystemComponent);
	if (!Item->ReduceStackCount())
	{
		RemoveItem(Item);
	}
	else
	{
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority())
		return;

	Item->RemoveGASModifications(OwnerAbilitySystemComponent);
	OnItemRemoved.Broadcast(Item->GetHandle());
	InventoryMap.Remove(Item->GetHandle());
	Client_ItemRemoved(Item->GetHandle());
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle ItemHandle)
{
	if (GetOwner()->HasAuthority())
		return;

	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem)
		return;

	OnItemRemoved.Broadcast(ItemHandle);
	InventoryMap.Remove(ItemHandle);
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
	if (GetOwner()->HasAuthority())
		return;

	UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle);
	if (FoundItem)
	{
		FoundItem->SetStackCount(NewCount);
		OnItemStackCountChanged.Broadcast(Handle, NewCount);
	}
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item)
{
	if (GetOwner()->HasAuthority())
		return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, Item);
	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	UE_LOG(LogTemp, Warning, TEXT("Client Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
}

void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(NewSlotNumber);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
	UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
	if (FoundItem)
	{
		return *FoundItem;
	}

	return nullptr;
}

bool UInventoryComponent::IsFullFor(const UPA_ShopItem* Item) const
{
	if (!Item)
		return false;

	if (IsAllSlotOccupied())
	{
		return GetAvaliableStackForItem(Item) == nullptr;
	}

	return false;
}

bool UInventoryComponent::IsAllSlotOccupied() const
{
	return InventoryMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvaliableStackForItem(const UPA_ShopItem* Item) const
{
	if (!Item->GetIsStackable())
		return nullptr;

	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (ItemPair.Value && ItemPair.Value->IsForItem(Item) && !ItemPair.Value->IsStackFull())
		{
			return ItemPair.Value;
		}
	}

	return nullptr;
}

bool UInventoryComponent::FoundIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients)
{
	const FItemCollection* Ingredients = UCAssetManager::Get().GetIngredientForItem(Item);
	if (!Ingredients)
		return false;

	bool bAllFound = true;
	for (const UPA_ShopItem* Ingredient : Ingredients->GetItems())
	{
		UInventoryItem* FoundItem = TryGetItemForShopItem(Ingredient);
		if (!FoundItem)
		{
			bAllFound = false;
			break;
		}

		OutIngredients.Add(FoundItem);
	}

	return bAllFound;
}

UInventoryItem* UInventoryComponent::TryGetItemForShopItem(const UPA_ShopItem* Item) const
{
	if (!Item)
		return nullptr;

	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemHandlePair : InventoryMap)
	{
		if (ItemHandlePair.Value && ItemHandlePair.Value->GetShopItem() == Item)
		{
			return ItemHandlePair.Value;
		}
	}

	return nullptr;
}

void UInventoryComponent::CheckItemCombination(const UInventoryItem* NewItem)
{
	if (!GetOwner()->HasAuthority())
		return;

	const FItemCollection* CombinationItems = UCAssetManager::Get().GetCombinationForItem(NewItem->GetShopItem());
	if (!CombinationItems)
		return;

	for (const UPA_ShopItem* CombinationItem : CombinationItems->GetItems())
	{
		TArray<UInventoryItem*> Ingredients;
		if (!FoundIngredientForItem(CombinationItem, Ingredients))
			continue;

		for (UInventoryItem* Ingredient : Ingredients)
		{
			RemoveItem(Ingredient);
		}

		GrantItem(CombinationItem);
		return;
	}
}