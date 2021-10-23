// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <Factories/Factory.h>

#include "ItemAssetFactory.generated.h"

UCLASS()
class UNREALINVENTORYEDITOR_API UItemAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UItemAssetFactory();

	virtual FString GetDefaultNewAssetName() const override { return "ID_Item"; }

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};


