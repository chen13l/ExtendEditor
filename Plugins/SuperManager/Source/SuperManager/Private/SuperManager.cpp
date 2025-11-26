// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitContentBrowserMenuExtension();
	RegisterAdvanceDeletionTab();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitContentBrowserMenuExtension()
{
	//get hold of all the menu extenders
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	//add custom delegate to all the existing delegates
	CBMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomCBMenuExtenderCallback));
}

// to define the position for interesting menu entry
TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtenderCallback(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			/* second binding, will define details for custom menu entry */
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry)
		);

		FolderPathSelected = SelectedPaths;
	}

	return MenuExtender;
}

// define details for the custom menu entry
void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete all unused assets")),
		FSlateIcon(),
		/* the actual function to execute */
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonClicked)
	);
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folder")),
		FText::FromString(TEXT("Safely delete all empty folder")),
		FSlateIcon(),
		/* the actual function to execute */
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFolderButtonClicked)
	);
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advanced Delete")),
		FText::FromString(TEXT("List assets by specific condition in a tab for deleting")),
		FSlateIcon(),
		/* the actual function to execute */
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnAdvancedDeleteButtonClicked)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathSelected.Num() > 1)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("Only can do this to one folder"));
		return;
	}

	TArray<FString> ListAssets = UEditorAssetLibrary::ListAssets(FolderPathSelected[0]);

	/* whether there are assets under the selected folder */
	if (ListAssets.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("No asset found under selected folder"));
		return;
	}

	EAppReturnType::Type MessageDialogReturnType =
		DebugHeader::ShowMessageDialog(EAppMsgType::YesNo,TEXT("A total of ")
		                               + FString::FromInt(ListAssets.Num())
		                               + TEXT(" found.\n Would you like to procceed?"));

	if (MessageDialogReturnType == EAppReturnType::No) { return; }

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetDataList;
	for (const FString& AssetPath : ListAssets)
	{
		if (AssetPath.Contains(TEXT("Developers")) || AssetPath.Contains(TEXT("Collections"))
			|| AssetPath.Contains(TEXT("__ExternalActors__")) || AssetPath.Contains(TEXT("__ExternalObjects__"))
			|| AssetPath.Contains(TEXT("Maps"))) { continue; }

		FString Tem;
		AssetPath.Split(TEXT("."), &Tem, nullptr, ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		if (!UEditorAssetLibrary::DoesAssetExist(Tem)) { continue; }

		TArray<FString> PackageRefForAsset = UEditorAssetLibrary::FindPackageReferencersForAsset(Tem);
		if (PackageRefForAsset.Num() == 0) { UnusedAssetDataList.Add(UEditorAssetLibrary::FindAssetData(Tem)); }
	}

	if (UnusedAssetDataList.Num() > 0) { ObjectTools::DeleteAssets(UnusedAssetDataList); }
	else { DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("No unused asset found under selected folder")); }
}

void FSuperManagerModule::OnDeleteEmptyFolderButtonClicked()
{
	FixUpRedirectors();

	TArray<FString> FolderPathArr = UEditorAssetLibrary::ListAssets(FolderPathSelected[0]);
	uint32 Count = 0;

	FString EmptyFolderPathName;
	TArray<FString> EmptyFolderPathArr;

	for (const FString& FolderPath : FolderPathArr)
	{
		if (FolderPath.Contains(TEXT("Developers")) || FolderPath.Contains(TEXT("Collections"))
			|| FolderPath.Contains(TEXT("__ExternalActors__")) || FolderPath.Contains(TEXT("__ExternalObjects__"))) { continue; }

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath)) { continue; }

		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathName.Append(FolderPath);
			EmptyFolderPathName.Append("\n");

			EmptyFolderPathArr.Add(FolderPath);
		}
	}

	if (EmptyFolderPathArr.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("No empty folder found"), false);
		return;
	}

	EAppReturnType::Type MessageDialogReturnType = DebugHeader::ShowMessageDialog(
		EAppMsgType::OkCancel,
		TEXT("Empty folder found in: ") + EmptyFolderPathName + TEXT("\nWould you like to procceed?")
		, false);

	if (MessageDialogReturnType == EAppReturnType::Cancel) { return; }

	for (const FString& EmptyFolderPath : EmptyFolderPathArr)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath)) { ++Count; }
		else { DebugHeader::PrintMessage(TEXT("Failed to delete ") + EmptyFolderPath, FColor::Red); }
	}

	if (Count > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(Count) + TEXT(" folders"));
	}
}

void FSuperManagerModule::OnAdvancedDeleteButtonClicked()
{
	FixUpRedirectors();
	
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedDeletion"));
}

void FSuperManagerModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> Redirectors;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add("/Game");
	Filter.ClassPaths.Emplace("/Script/ObjectRedirector");

	TArray<FAssetData> OutAssetDatas;
	AssetRegistryModule.Get().GetAssets(Filter, OutAssetDatas);

	for (FAssetData& AssetData : OutAssetDatas)
	{
		if (UObjectRedirector* Redirector = Cast<UObjectRedirector>(AssetData.GetAsset()))
		{
			Redirectors.AddUnique(Redirector);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(Redirectors);
}


#pragma endregion ContentBrowserMenuExtension


#pragma region CustomEditorTab
void FSuperManagerModule::RegisterAdvanceDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName(TEXT("AdvancedDeletion")),
		FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnAdvanceDeletionTab)
	).SetDisplayName(FText::FromString(TEXT("Advanced Deletion")));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(NomadTab)
		[
			SNew(SAdvanceDeletionTab)
			.AssetDatasToStored(GetAllAssetsDataUnderSelectedFolder())
			.CurrentSelectedFolder(FolderPathSelected[0])
		];
}

TArray<TSharedPtr<FAssetData>> FSuperManagerModule::GetAllAssetsDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetDatas;

	TArray<FString> ListAssets = UEditorAssetLibrary::ListAssets(FolderPathSelected[0]);

	for (const FString& AssetPath : ListAssets)
	{
		if (AssetPath.Contains(TEXT("Developers")) || AssetPath.Contains(TEXT("Collections"))
			|| AssetPath.Contains(TEXT("__ExternalActors__")) || AssetPath.Contains(TEXT("__ExternalObjects__"))
			|| AssetPath.Contains(TEXT("Maps"))) { continue; }

		FString Tem;
		AssetPath.Split(TEXT("."), &Tem, nullptr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

		if (!UEditorAssetLibrary::DoesAssetExist(Tem)) { continue; }

		const FAssetData FindAssetData = UEditorAssetLibrary::FindAssetData(Tem);

		AvailableAssetDatas.AddUnique(MakeShared<FAssetData>(FindAssetData));
	}

	return AvailableAssetDatas;
}

#pragma endregion CustomEditorTab

#pragma region ProccessDataForAssetList
bool FSuperManagerModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	TArray<FAssetData> AssetDatas;
	AssetDatas.Add(AssetDataToDelete);

	if (ObjectTools::DeleteAssets(AssetDatas) > 0) { return true; }

	return false;
}

bool FSuperManagerModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete) > 0) { return true; }

	return false;
}

void FSuperManagerModule::ListUnusedAssetsForAssetList(TArray<TSharedPtr<FAssetData>>& AssetDatasToFilter,
                                                       TArray<TSharedPtr<FAssetData>>& OutAssetDatas)
{
	OutAssetDatas.Empty();

	for (const TSharedPtr<FAssetData>& AssetData : AssetDatasToFilter)
	{
		TArray<FString> RefForAsset = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData->GetSoftObjectPath().GetAssetPathString());

		if (RefForAsset.Num() == 0)
		{
			OutAssetDatas.AddUnique(AssetData);
		}
	}
}

void FSuperManagerModule::ListSameNameAssetsForAssetList(TArray<TSharedPtr<FAssetData>>& AssetDatasToFilter,
	TArray<TSharedPtr<FAssetData>>& OutAssetDatas)
{
	TArray<TSharedPtr<FAssetData>> OutDatas;
	
	TMap<FName, TArray<TSharedPtr<FAssetData>>> AssetInfoMap;

	for (const TSharedPtr<FAssetData>& AssetData : AssetDatasToFilter)
	{
		FName AssetName = AssetData->AssetName;
		if (AssetInfoMap.Contains(AssetName)){AssetInfoMap[AssetName].Add(AssetData);}
	}

	for (const auto& [AssetName, AssetDatas] : AssetInfoMap)
	{
		if (AssetDatas.Num() <= 1){continue;}

		for (const TSharedPtr<FAssetData>& AssetData : AssetDatas)
		{
			OutDatas.AddUnique(AssetData);
		}
	}
}

void FSuperManagerModule::SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync)
{
	TArray<FString> AssetPath;
	AssetPath.Add(AssetPathToSync);

	UEditorAssetLibrary::SyncBrowserToObjects(AssetPath);
}

#pragma endregion ProccessDataForAssetList

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)
