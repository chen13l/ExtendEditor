#pragma once
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void PrintMessage(const FString& Message, const FColor& Color = FColor::Red)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, Color, Message);
	}
}

void LogMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

EAppReturnType::Type ShowMessageDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMessageAsWarning = true)
{
	if (bShowMessageAsWarning)
	{
		const FText& Title = FText::FromString("Warning");
		return FMessageDialog::Open(MsgType, FText::FromString(Message), Title);
	}
	return FMessageDialog::Open(MsgType, FText::FromString(Message));
}

void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.bUseLargeFont = true;
	Info.FadeOutDuration = 7.f;

	FSlateNotificationManager::Get().AddNotification(Info);
}