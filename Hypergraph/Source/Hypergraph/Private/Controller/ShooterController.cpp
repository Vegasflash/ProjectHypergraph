// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ShooterController.h"
#include "HUD/ShooterHUD.h"
#include "HUD//CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/BaseCharacter.h"
#include "PlayerState/ShooterPlayerState.h"

void AShooterController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHUD = Cast<AShooterHUD>(GetHUD());
}

void AShooterController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (ABaseCharacter* ShooterCharacter = Cast<ABaseCharacter>(aPawn))
	{
		SetHUDHealth(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
	}
}

void AShooterController::SetHUDHealth(float Health, float MaxHealth)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD)
	{
		if (UCharacterOverlay* CharacterOverlay = ShooterHUD->GetCharacterOverlay())
		{
			const float HealthPercent = MaxHealth < 0.1 ? 0 : Health / MaxHealth; // potential 0 div.
			CharacterOverlay->GetHealthBar()->SetPercent(HealthPercent);

			FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
			CharacterOverlay->GetHealthText()->SetText(FText::FromString(HealthText));
			return;
		}
	}

}

void AShooterController::SetHUDKills(float Kills)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		UCharacterOverlay* CharacterOverlay = ShooterHUD->GetCharacterOverlay();

		auto KillsText = CharacterOverlay->GetKillsAmountText();
		if (CharacterOverlay && KillsText)
		{
			KillsText->SetText(FText::AsNumber(FMath::RoundToInt(Kills)));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Null refs in Hud, need to fix this."));
		}
	}
}

void AShooterController::SetHUDDeaths(float Deaths)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		UCharacterOverlay* CharacterOverlay = ShooterHUD->GetCharacterOverlay();

		auto DeathText = CharacterOverlay->GetDeathAmountText();
		if (CharacterOverlay && DeathText)
		{
			DeathText->SetText(FText::AsNumber(FMath::RoundToInt(Deaths)));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Null refs in Hud, need to fix this."));
		}
	}
	
}


