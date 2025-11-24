#include "SlateWidgets/AdvancedDeletionWidget.h"

#include "DebugHeader.h"

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetDatas = InArgs._AssetDatasToStored;

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
		]

		// slot for asset list
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)

			+ SScrollBox::Slot()
			[
				SNew(SListView<TSharedPtr<FAssetData>>)
				.ItemHeight(24.f)
				.ListItemsSource(&StoredAssetDatas)
				.OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateListRow)
			]
		]

		// 3 buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
}

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateListRow(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid()) { return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable); }

	const FString AssetName = AssetDataToDisplay->AssetName.ToString();
	FSlateFontInfo AssetNameFontInfo = GetFontInfo();
	AssetNameFontInfo.Size = 15;

	FString AssetClassName;
	AssetDataToDisplay->AssetClassPath.ToString().Split(TEXT("."), nullptr, &AssetClassName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FSlateFontInfo AssetClassNameFontInfo = GetFontInfo();
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

	return ConstructCheckBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		DebugHeader::PrintMessage(AssetData->AssetName.ToString() + TEXT(" Unchecked"), FColor::Red);
		break;
	case ECheckBoxState::Checked:
		DebugHeader::PrintMessage(AssetData->AssetName.ToString() + TEXT(" Checked"), FColor::Green);
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
	return FReply::Handled();
}
