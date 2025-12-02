#include "CustomStyle/SuperManagerStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FSuperManagerStyle::StyleSetName = FName("SuperManagerStyle");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedSlateStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcon()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomSlateStyleSet = MakeShareable<FSlateStyleSet>(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin("SuperManager")->GetBaseDir() / "Resources";

	CustomSlateStyleSet->SetContentRoot(IconDirectory);

	const FVector2d Icon16x16(16.f, 16.f);
	
	CustomSlateStyleSet->Set(
		"ContentBrowser.DeleteUnusedAsset"
		, new FSlateImageBrush(IconDirectory / "DeleteUnusedAsset.png", Icon16x16));
	
	CustomSlateStyleSet->Set(
		"ContentBrowser.DeleteEmptyFolder"
		, new FSlateImageBrush(IconDirectory / "DeleteEmptyFolder.png", Icon16x16));
	
	CustomSlateStyleSet->Set(
		"ContentBrowser.AdvanceDeletion"
		, new FSlateImageBrush(IconDirectory / "AdvanceDeletion.png", Icon16x16));

	return CustomSlateStyleSet;
}

void FSuperManagerStyle::ShutDown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}
