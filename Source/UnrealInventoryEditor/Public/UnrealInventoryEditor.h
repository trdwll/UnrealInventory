// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>
#include <Styling/SlateStyle.h>

class FUnrealInventoryEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

	TSharedPtr<FSlateStyleSet> m_StyleSet;

	TArray<TSharedPtr<IAssetTypeActions>> m_AssetTypeActions;
};
