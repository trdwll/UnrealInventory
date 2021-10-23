// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetActions/ItemAssetAction.h"
#include <AssetTypeCategories.h>

#include "ItemDataAsset.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_ItemAsset"

FColor FAssetTypeActions_ItemAsset::GetTypeColor() const
{
	return FColor::Green;
}

UClass* FAssetTypeActions_ItemAsset::GetSupportedClass() const
{
	return UCItemDescriptor::StaticClass();
}

uint32 FAssetTypeActions_ItemAsset::GetCategories()
{
	return EAssetTypeCategories::Gameplay | EAssetTypeCategories::Misc;
}

FText FAssetTypeActions_ItemAsset::GetName() const
{
	return NSLOCTEXT("AssetTypeActions_ItemAsset", "AssetTypeActions_ItemAsset", "InventoryItemAsset");
}

#undef LOCTEXT_NAMESPACE