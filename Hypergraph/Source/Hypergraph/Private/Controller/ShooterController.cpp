// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ShooterController.h"
#include "HUD/ShooterHUD.h"
#include "HUD//CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void AShooterController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHUD = Cast<AShooterHUD>(GetHUD());
}


void AShooterController::SetHUDHealth(float Health, float MaxHealth)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD)
	{
		if (UCharacterOverlay* CharacterOverlay = ShooterHUD->GetCharacterOverlay())
		{
			const float HealthPercent = Health / MaxHealth;
			CharacterOverlay->GetHealthBar()->SetPercent(HealthPercent);

			FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
			CharacterOverlay->GetHealthText()->SetText(FText::FromString(HealthText));
			return;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Null refs in Hud, need to fix this."))
		}
	}

}