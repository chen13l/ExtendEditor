#include "SlateWidgets/AdvancedDeletionWidget.h"

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
		.AutoHeight()
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
	const FString AssetName = AssetDataToDisplay->AssetName.ToString();
	
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(AssetName))
		];

	return ListViewRowWidget;
}
