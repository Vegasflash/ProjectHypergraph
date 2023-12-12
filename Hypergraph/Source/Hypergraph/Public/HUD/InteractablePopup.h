// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Character/BaseCharacter.h"

#include "InteractablePopup.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UInteractablePopup : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UBorder> Border;
};
