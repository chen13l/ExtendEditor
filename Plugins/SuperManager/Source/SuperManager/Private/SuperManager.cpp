// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitContentBrowserMenuExtension();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitContentBrowserMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	CBMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomCBMenuExtenderCallback));
}

TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtenderCallback(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry));

		FolderPathSelected = SelectedPaths;
	}

	return MenuExtender;
}

void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Unused Assets")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonClicked)
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

	TArray<FAssetData> UnusedAssetDataList;
	for (const FString& AssetPath : ListAssets)
	{
		if (AssetPath.Contains(TEXT("Developers")) || AssetPath.Contains(TEXT("Collections"))
			|| AssetPath.Contains(TEXT("__ExternalActors__")) || AssetPath.Contains(TEXT("__ExternalObjects__"))) { continue; }

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPath)) { continue; }

		TArray<FString> PackageRefForAsset = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath);
		if (PackageRefForAsset.Num() == 0) { UnusedAssetDataList.Add(UEditorAssetLibrary::FindAssetData(AssetPath)); }
	}

	if (UnusedAssetDataList.Num() > 0) { ObjectTools::DeleteAssets(UnusedAssetDataList); }
	else { DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("No unused asset found under selected folder")); }
}


#pragma endregion ContentBrowserMenuExtension


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)
