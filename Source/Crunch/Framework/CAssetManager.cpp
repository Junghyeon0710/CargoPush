// Fill out your copyright notice in the Description page of Project Settings.


#include "CAssetManager.h"

#include "Crunch/Inventory/PA_ShopItem.h"

UCAssetManager& UCAssetManager::Get()
{
	UCAssetManager* AssetManager = Cast<UCAssetManager>(GEngine->AssetManager.Get());
	if (AssetManager)
	{
		return *AssetManager;
	}

	UE_LOG(LogLoad, Fatal, TEXT("Asset Manager Needs to be of the type CASssetManager"));

	return *NewObject<UCAssetManager>();
}

void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished, LoadFinishedCallback));
}

bool UCAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObject;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObject);

	if(bLoaded)
	{
		for (UObject* Object : LoadedObject)
		{
			OutItems.Add(Cast<UPA_ShopItem>(Object));
		}
	}

	return bLoaded;
}

const FItemCollection* UCAssetManager::GetCombinationForItem(const UPA_ShopItem* Item) const
{
	return CombinationMap.Find(Item);
}

const FItemCollection* UCAssetManager::GetIngredientForItem(const UPA_ShopItem* Item) const
{
	return IngredientMap.Find(Item);
}


void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
	
}

void UCAssetManager::BuildItemMaps()
{
	TArray<const UPA_ShopItem*> LoadedItems;
	if (GetLoadedShopItems(LoadedItems))
	{
		for (const UPA_ShopItem* Item : LoadedItems)
		{
			if (Item->GetIngredientItems().Num() == 0)
			{
				continue;
			}

			TArray<const UPA_ShopItem*> Items;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredientItems())
			{
				UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				Items.Add(IngredientItem);
				AddToCombinationMap(IngredientItem, Item);
			}

			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

void UCAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	FItemCollection* Combinations = CombinationMap.Find(Ingredient);
	if (Combinations)
	{
		if (!Combinations->Contains(CombinationItem))
		{
			CombinationMap.Add(CombinationItem);
		}
	}
	else
	{
		CombinationMap.Add(Ingredient, FItemCollection{TArray<const UPA_ShopItem*>{CombinationItem}});
	}
}
