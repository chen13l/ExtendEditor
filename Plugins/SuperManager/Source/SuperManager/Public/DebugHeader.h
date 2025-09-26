#pragma once

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
