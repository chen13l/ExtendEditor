#include "AssetActions/QuickAssetAction.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UQuickAssetAction::ExecuteAction()
{
	DebugHeader::PrintMessage("Executing Quick Asset Action", FColor::Blue);
	DebugHeader::LogMessage("Working");
}

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> AssetDatas = UEditorUtilityLibrary::GetSelectedAssetData();
	int32 Counter = 0;

	for (const FAssetData& AssetData : AssetDatas)
	{
		const FString& SourceAssetPath = AssetData.GetSoftObjectPath().ToString();
		for (int i = 0; i < NumOfDuplicates; i++)
		{
			const FString& NewDuplicatedAssetName = AssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
			const FString& NewAssetPath = FPaths::Combine(AssetData.PackagePath.ToString(), NewDuplicatedAssetName);

			if (FPaths::FileExists(NewAssetPath))
			{
				DebugHeader::PrintMessage(FString::Printf(TEXT("%s has existed"), *NewAssetPath), FColor::Red);
				continue;
			}

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewAssetPath))
			{
				UEditorAssetLibrary::SaveAsset(NewAssetPath, false);
				++Counter;
			}
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated ") + FString::FromInt(Counter) + " files");
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	int32 Counter = 0;

	for (UObject* Object : SelectedObjects)
	{
		if (!IsValid(Object)) { continue; }

		FString* Prefix = PrefixMap.Find(Object->GetClass());
		if (!Prefix || Prefix->IsEmpty())
		{
			DebugHeader::PrintMessage("Failed to find prefix for class", FColor::Red);
			continue;
		}

		FString ObjectName = Object->GetName();
		if (ObjectName.StartsWith(*Prefix))
		{
			DebugHeader::PrintMessage("prefix already added", FColor::Yellow);
			continue;
		}

		if (Object->IsA<UMaterialInstanceConstant>())
		{
			ObjectName.RemoveFromStart(TEXT("M_"));
			ObjectName.RemoveFromEnd(TEXT("_Inst"));
		}

		const FString NewName = *Prefix + ObjectName;
		UEditorUtilityLibrary::RenameAsset(Object, NewName);

		++Counter;
	}

	if (Counter > 0) { DebugHeader::ShowNotifyInfo(TEXT("Successfully rename "+FString::FromInt(Counter) + " assets")); }
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssets;

	FixUpRedirectors();

	for (const FAssetData& AssetData : SelectedAssetData)
	{
		TArray<FString> AssetRefs = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData.GetObjectPathString());
		if (AssetRefs.Num() == 0)
		{
			UnusedAssets.AddUnique(AssetData);
		}
	}

	if (UnusedAssets.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("No unused asset"), false);
		return;
	}

	int32 DeleteAssetsNum = ObjectTools::DeleteAssets(UnusedAssets);
	if (DeleteAssetsNum == 0){return;}
	DebugHeader::ShowNotifyInfo(TEXT("Successfully delete ") + FString::FromInt(DeleteAssetsNum) + " assets");
}

void UQuickAssetAction::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFix;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add("/Game");
	Filter.ClassPaths.Emplace("/Script/ObjectRedirector");

	TArray<FAssetData> OutAssetData;
	AssetRegistryModule.Get().GetAssets(Filter,OutAssetData);

	for (const FAssetData& RedirectAssetData : OutAssetData)
	{
		if (UObjectRedirector* Redirector = Cast<UObjectRedirector>(RedirectAssetData.GetAsset()))
		{
			RedirectorsToFix.AddUnique(Redirector);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
}
