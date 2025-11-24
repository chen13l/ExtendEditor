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

	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructListView();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedListView;

	TSharedRef<ITableRow> OnGenerateListRow(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);

	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	TSharedRef<STextBlock> ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontInfo);
	FORCEINLINE FSlateFontInfo GetFontInfo() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }

	TSharedRef<SButton> ConstructButton(const TSharedPtr<FAssetData>& AssetData);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
	void RefreshAssetListView();
};
