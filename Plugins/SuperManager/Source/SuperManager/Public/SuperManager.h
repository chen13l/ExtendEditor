// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
#pragma region ContentBrowserMenuExtension
	void InitContentBrowserMenuExtension();
	
	TArray<FString> FolderPathSelected;
	TSharedRef<FExtender> CustomCBMenuExtenderCallback(const TArray<FString>& SelectedPaths);

	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetButtonClicked();
	void OnDeleteEmptyFolderButtonClicked();
	void OnAdvancedDeleteButtonClicked();

	void FixUpRedirectors();
#pragma endregion ContentBrowserMenuExtension

#pragma region CustomEditorTab
	void RegisterAdvanceDeletionTab();

	TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs);

	TArray<TSharedPtr<FAssetData>> GetAllAssetsDataUnderSelectedFolder();
#pragma endregion CustomEditorTab

public:
#pragma region ProccessDataForAssetList
	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
#pragma endregion ProccessDataForAssetList
};
