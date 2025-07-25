// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItem.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, UInventoryItem* /*NewItem*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	FOnItemAddedDelegate OnItemAdded;
	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	float GetGold() const;

protected:

	virtual void BeginPlay() override;

public:

private:	
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	/*********************************************************/
	/*                   Server                              */
	/*********************************************************/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);
	void GrantItem(const UPA_ShopItem* NewItem);

	/*********************************************************/
	/*                   Client                              */
	/*********************************************************/
private:
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item);
};
