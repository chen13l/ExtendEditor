#include "AssetActions/QuickAssetAction.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

void UQuickAssetAction::ExecuteAction()
{
	PrintMessage("Executing Quick Asset Action", FColor::Blue);
	LogMessage("Working");
}

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		ShowMessageDialog(EAppMsgType::Ok,TEXT("Please enter a VALID number"));
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
				PrintMessage(FString::Printf(TEXT("%s has existed"), *NewAssetPath), FColor::Red);
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
		ShowNotifyInfo(TEXT("Successfully duplicated ") + FString::FromInt(Counter) + " files");
	}
}
