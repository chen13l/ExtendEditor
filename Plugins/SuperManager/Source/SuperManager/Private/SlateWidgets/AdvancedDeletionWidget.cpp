#include "SlateWidgets/AdvancedDeletionWidget.h"

#include "DebugHeader.h"
#include "SuperManager.h"

#define ListAll TEXT("List all available assets")
#define ListUnused TEXT("List all available unused assets")

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	CheckBoxArr.Empty();
	SelectedAssetDatas.Empty();

	ComboBoxSourceItems.AddUnique(MakeShared<FString>(ListAll));
	ComboBoxSourceItems.AddUnique(MakeShared<FString>(ListUnused));

	StoredAssetDatas = InArgs._AssetDatasToStored;
	DisplayAssetDatas = StoredAssetDatas;

	FSlateFontInfo TitleFontInfo = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleFontInfo.Size = 30;

	ChildSlot
	[
		// main vertical box
		SNew(SVerticalBox)

		// title text
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleFontInfo)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]

		// slot for drop down to specify the listing condition and help text
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// combo box slot
			+ SHorizontalBox::Slot()
			[
				ConstructComboBox()
			]
		]

		// slot for asset list
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)

			+ SScrollBox::Slot()
			[
				ConstructListView()
			]
		]

		// 3 buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteAllButton()
			]

			+ SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructSelectAllButton()
			]

			+ SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeselectAllButton()
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructListView()
{
	ConstructedListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(24.f)
		.ListItemsSource(&DisplayAssetDatas)
		.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateListRow);

	return ConstructedListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	SelectedAssetDatas.Empty();
	CheckBoxArr.Empty();

	if (ConstructedListView.IsValid())
	{
		ConstructedListView->RebuildList();
	}
}

#pragma region ComboBoxForListingCondition
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ComboBoxSourceItems)
		.OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboBoxContent)
		[
			SAssignNew(ComboBoxDisplayText, STextBlock)
			.Text(FText::FromString(TEXT("List Assets Options")))
		];

	return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboBoxContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<STextBlock> ConstructedTextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(*SourceItem));

	return ConstructedTextBlock;
}


void SAdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	ComboBoxDisplayText->SetText(FText::FromString(*SelectedOption));

	DebugHeader::PrintMessage(*SelectedOption);

	FSuperManagerModule& SuperManagerModule = FModuleManager::Get().LoadModuleChecked<FSuperManagerModule>("SuperManager");

	// pass data to filter base on the selecting option
	if (*SelectedOption.Get() == ListAll)
	{
		DisplayAssetDatas = StoredAssetDatas;
		RefreshAssetListView();
	}
	else if (*SelectedOption.Get() == ListUnused)
	{
		SuperManagerModule.ListUnusedAssetsForAssetList(StoredAssetDatas, DisplayAssetDatas);
		RefreshAssetListView();
	}
}
#pragma endregion ComboBoxForListingCondition

#pragma region RowWidgetForAssetListView
TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateListRow(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid()) { return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable); }

	const FString AssetName = AssetDataToDisplay->AssetName.ToString();
	FSlateFontInfo AssetNameFontInfo = GetEmbossedTextFont();
	AssetNameFontInfo.Size = 15;

	FString AssetClassName;
	AssetDataToDisplay->AssetClassPath.ToString().Split(TEXT("."), nullptr, &AssetClassName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FSlateFontInfo AssetClassNameFontInfo = GetEmbossedTextFont();
	AssetClassNameFontInfo.Size = 10;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(5.f))
		[
			SNew(SHorizontalBox)

			// slot for check box
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.FillWidth(.05f)
			[
				ConstructCheckBox(AssetDataToDisplay)
			]

			// slot for displaying asset class name
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Fill)
			.FillWidth(.5f)
			[
				ConstructTextBlock(AssetClassName, AssetClassNameFontInfo)
			]

			// slot for displaying asset name
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Fill)
			[
				ConstructTextBlock(AssetName, AssetNameFontInfo)
			]

			// slot for a delete button
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			[
				ConstructButton(AssetDataToDisplay)
			]

		];

	return ListViewRowWidget;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructCheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);

	CheckBoxArr.AddUnique(ConstructCheckBox);

	return ConstructCheckBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		DebugHeader::PrintMessage(AssetData->AssetName.ToString() + TEXT(" Unchecked"), FColor::Red);

		if (SelectedAssetDatas.Contains(AssetData)) { SelectedAssetDatas.Remove(AssetData); }
		break;

	case ECheckBoxState::Checked:
		DebugHeader::PrintMessage(AssetData->AssetName.ToString() + TEXT(" Checked"), FColor::Green);

		SelectedAssetDatas.AddUnique(AssetData);
		break;

	case ECheckBoxState::Undetermined:
		break;

	default:
		break;
	}
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextBlock(const FString& TextContent, const FSlateFontInfo& FontInfo)
{
	TSharedRef<STextBlock> ConstructTextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontInfo)
		.ColorAndOpacity(FColor::White);

	return ConstructTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButton(const TSharedPtr<FAssetData>& AssetData)
{
	TSharedRef<SButton> ConstructButton =
		SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.OnClicked(this, &SAdvanceDeletionTab::OnDeleteButtonClicked, AssetData);

	return ConstructButton;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	DebugHeader::PrintMessage(ClickedAssetData->AssetName.ToString() + TEXT(" Clicked"), FColor::Green);

	FSuperManagerModule& SuperManagerModule = FModuleManager::Get().LoadModuleChecked<FSuperManagerModule>("SuperManager");
	const bool bAssetDeleted = SuperManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

	if (bAssetDeleted)
	{
		if (StoredAssetDatas.Contains(ClickedAssetData))
		{
			StoredAssetDatas.Remove(ClickedAssetData);
		}

		RefreshAssetListView();
	}

	return FReply::Handled();
}
#pragma endregion RowWidgetForAssetListView

#pragma region TabButtons
TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> ConstructDeleteAllButton =
		SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &SAdvanceDeletionTab::OnDeleteAllButtonClicked);

	ConstructDeleteAllButton->SetContent(ConstructTextForTabButton(TEXT("Delete All")));

	return ConstructDeleteAllButton;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	if (SelectedAssetDatas.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok,TEXT("No asset currently selected"));
		return FReply::Handled();
	}

	TArray<FAssetData> AssetDatasToDelete;

	for (const TSharedPtr<FAssetData> AssetDataPtr : SelectedAssetDatas)
	{
		AssetDatasToDelete.AddUnique(*AssetDataPtr.Get());
	}

	FSuperManagerModule& SuperManagerModule = FModuleManager::Get().LoadModuleChecked<FSuperManagerModule>("SuperManager");
	const bool bAssetsDeleted = SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetDatasToDelete);
	if (bAssetsDeleted)
	{
		for (const TSharedPtr<FAssetData> AssetDataPtr : SelectedAssetDatas)
		{
			if (StoredAssetDatas.Contains(AssetDataPtr)) { StoredAssetDatas.Remove(AssetDataPtr); }

			if (DisplayAssetDatas.Contains(AssetDataPtr)) { DisplayAssetDatas.Remove(AssetDataPtr); }
		}
		RefreshAssetListView();
	}

	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> ConstructSelectAllButton =
		SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &SAdvanceDeletionTab::OnSelectAllButtonClicked);

	ConstructSelectAllButton->SetContent(ConstructTextForTabButton(TEXT("Select All")));

	return ConstructSelectAllButton;
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	if (CheckBoxArr.Num() == 0) { return FReply::Handled(); }

	for (const TSharedRef<SCheckBox>& CheckBoxRef : CheckBoxArr)
	{
		if (!CheckBoxRef->IsChecked())
		{
			CheckBoxRef->ToggleCheckedState();
		}
	}

	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> ConstructDeselectAllButton =
		SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &SAdvanceDeletionTab::OnDeselectAllButtonClicked);

	ConstructDeselectAllButton->SetContent(ConstructTextForTabButton(TEXT("DeSelect All")));

	return ConstructDeselectAllButton;
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
	if (CheckBoxArr.Num() == 0) { return FReply::Handled(); }

	for (const TSharedRef<SCheckBox>& CheckBoxRef : CheckBoxArr)
	{
		if (CheckBoxRef->IsChecked())
		{
			CheckBoxRef->ToggleCheckedState();
		}
	}

	return FReply::Handled();
}
#pragma endregion TabButtons

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForTabButton(const FString& TextContent)
{
	FSlateFontInfo FontInfo = GetEmbossedTextFont();
	FontInfo.Size = 15;

	TSharedRef<STextBlock> ConstructTextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontInfo)
		.Justification(ETextJustify::Center);

	return ConstructTextBlock;
}
