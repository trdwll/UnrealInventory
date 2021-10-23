// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <AssetTypeActions_Base.h>
#include <AssetTypeCategories.h>
#include <CoreMinimal.h>
#include <Templates/SharedPointer.h>

class FAssetTypeActions_ItemAsset : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
};
