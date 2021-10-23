// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealInventoryEditor.h"

#include "AssetActions/ItemAssetAction.h"

#include <Interfaces/IPluginManager.h>
#include <Styling/SlateStyleRegistry.h>

#define LOCTEXT_NAMESPACE "FUnrealInventoryEditorModule"

void FUnrealInventoryEditorModule::StartupModule()
{
	m_StyleSet = MakeShareable(new FSlateStyleSet("UnrealInventoryStyle"));

	const FString ContentDir = IPluginManager::Get().FindPlugin("UnrealInventory")->GetBaseDir();

	m_StyleSet->SetContentRoot(ContentDir);

	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(m_StyleSet->RootToContentDir(TEXT("Resources/Editor/Items/Item"), TEXT(".png")), FVector2D(128.f, 128.f));

	if (ThumbnailBrush)
	{
		m_StyleSet->Set("ClassThumbnail.CItemDataAsset", ThumbnailBrush);

		FSlateStyleRegistry::RegisterSlateStyle(*m_StyleSet);
	}

	// Asset Actions
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_ItemAsset()));
}

void FUnrealInventoryEditorModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(m_StyleSet->GetStyleSetName());

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Unregister our custom created assets from the AssetTools
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto I : m_AssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(I.ToSharedRef());
		}
	}

	m_AssetTypeActions.Empty();
}

void FUnrealInventoryEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	m_AssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealInventoryEditorModule, UnrealInventoryEditor)
