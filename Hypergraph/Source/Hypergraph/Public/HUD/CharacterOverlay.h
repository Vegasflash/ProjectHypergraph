// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UTextBlock* HealthText;

public:
	FORCEINLINE UProgressBar* GetHealthBar() const { return HealthBar; }
	FORCEINLINE UTextBlock* GetHealthText() const { return HealthText; }
};
