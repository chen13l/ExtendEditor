#pragma once

class SAdvanceDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvanceDeletionTab)
		{
		};
		SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetDatasToStored);
	SLATE_END_ARGS()

public:
	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	void Construct(const FArguments& InArgs);
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

private:
	TArray<TSharedPtr<FAssetData>> StoredAssetDatas;
	TArray<TSharedPtr<FAssetData>> DisplayAssetDatas;

	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructListView();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedListView;

	TArray<TSharedRef<SCheckBox>> CheckBoxArr;

	TArray<TSharedPtr<FAssetData>> SelectedAssetDatas;

	void RefreshAssetListView();

#pragma region ComboBoxForListingCondition
	TArray<TSharedPtr<FString>> ComboBoxSourceItems;
	TSharedPtr<STextBlock> ComboBoxDisplayText;
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();

	TSharedRef<SWidget> OnGenerateComboBoxContent(TSharedPtr<FString> InItemText);
	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);

#pragma endregion ComboBoxForListingCondition

#pragma region RowWidgetForAssetListView
	TSharedRef<ITableRow> OnGenerateListRow(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	void OnRowWidgetMouseButtonClicked(TSharedPtr<FAssetData> ClickedData);

	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	TSharedRef<STextBlock> ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontInfo);

	TSharedRef<SButton> ConstructButton(const TSharedPtr<FAssetData>& AssetData);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
#pragma endregion RowWidgetForAssetListView

#pragma region TabButtons
	TSharedRef<SButton> ConstructDeleteAllButton();
	TSharedRef<SButton> ConstructSelectAllButton();
	TSharedRef<SButton> ConstructDeselectAllButton();

	FReply OnDeleteAllButtonClicked();
	FReply OnSelectAllButtonClicked();
	FReply OnDeselectAllButtonClicked();

	TSharedRef<STextBlock> ConstructTextForTabButton(const FString& TextContent);
#pragma endregion TabButtons

	FORCEINLINE FSlateFontInfo GetEmbossedTextFont() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }
};
