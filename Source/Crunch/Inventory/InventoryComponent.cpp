// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "PA_ShopItem.h"
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

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase)
		return;

	if (GetGold() < ItemToPurchase->GetPrice())
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

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
	InventoryItem->InitItem(NewHandle, NewItem);
	InventoryMap.Add(NewHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
	Client_ItemAdded(NewHandle, NewItem);
	InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent);
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
