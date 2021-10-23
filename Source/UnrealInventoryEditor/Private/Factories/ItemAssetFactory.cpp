// Copyright Epic Games, Inc. All Rights Reserved.

#include "Factories/ItemAssetFactory.h"

#include "ItemDataAsset.h"

UItemAssetFactory::UItemAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCItemDescriptor::StaticClass();
}

UObject* UItemAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCItemDescriptor>(InParent, InClass, InName, Flags);
}
