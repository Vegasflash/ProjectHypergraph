// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "AnnouncementPopup.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UAnnouncementPopup : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadonly, meta = (BindWidget, AllowPrivateAccess = true))
	class UTextBlock* Text;

public:
	void SetText(FText _Text);
	FORCEINLINE const FText GetText() const;
};
