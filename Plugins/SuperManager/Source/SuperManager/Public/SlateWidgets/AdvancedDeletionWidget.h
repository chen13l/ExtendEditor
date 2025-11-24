#pragma once

class SAdvanceDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvanceDeletionTab)
		{
		};
		SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDatasToStored);
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TArray<TSharedPtr<FAssetData>> StoredAssetDatas;

	TSharedRef<ITableRow> OnGenerateListRow(TSharedPtr<FAssetData> AssetDataToDisplay,const TSharedRef<STableViewBase>& OwnerTable);
};
