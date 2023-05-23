// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::ShowPlayerNetRole(APawn* Pawn)
{
	const ENetRole RemoteRole = Pawn->GetRemoteRole();
	const FString RoleName = StaticEnum<ENetRole>()->GetValueAsString(RemoteRole);

	FString PlayerName = "OtherPlayer";
	if(const APlayerState* PlayerState = Pawn->GetPlayerState())
	{
		PlayerName = PlayerState->GetPlayerName();	
	}
	
	SetDisplayText(FString::Printf(TEXT("%s : %s"), *PlayerName, *RoleName));
}

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
