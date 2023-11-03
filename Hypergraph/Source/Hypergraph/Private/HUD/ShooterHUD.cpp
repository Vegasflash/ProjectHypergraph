// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ShooterHUD.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		
		if (HUDPackage.CrosshairTextureMap.IsEmpty()) return;

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		for (int i = 0; i < (int)ECrosshairPosition::Count; ++i)
		{
			auto CurPosition = (ECrosshairPosition)i;
			auto Texture = HUDPackage.CrosshairTextureMap[CurPosition];

			if (Texture != nullptr)
			{
				FVector2D Spread(0.f, 0.f);
				switch (CurPosition)
				{
				case ECrosshairPosition::ECP_Top:
					Spread = FVector2D(0.f, -SpreadScaled);
					break;
				case ECrosshairPosition::ECP_Bottom:
					Spread = FVector2D(0.f, SpreadScaled);
					break;
				case ECrosshairPosition::ECP_Left:
					Spread = FVector2D(-SpreadScaled, 0.f);
					break;
				case ECrosshairPosition::ECP_Right:
					Spread = FVector2D(SpreadScaled, 0.f);
					break;
				case ECrosshairPosition::ECP_Center:
				default:
					break;
				}

				DrawCrosshair(Texture, ViewportCenter, Spread, HUDPackage.CrosshairColor);
			}
		}
	}
}

void AShooterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2) + Spread.Y);

	DrawTexture(Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor);
}
