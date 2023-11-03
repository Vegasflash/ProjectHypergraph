// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameEnums.h"
#include "ShooterHUD.generated.h"


USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	TMap<ECrosshairPosition, UTexture2D*> CrosshairTextureMap;

	float CrosshairSpread;
	FLinearColor CrosshairColor;
};

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

	virtual void DrawHUD() override;
	
private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere, Category="Crosshair")
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};