// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AnnouncementPopup.h"
#include "Components/TextBlock.h"

void UAnnouncementPopup::SetText(FText _Text)
{
	Text->SetText(_Text);
}

const FText UAnnouncementPopup::GetText() const
{
	return Text->GetText();
}
