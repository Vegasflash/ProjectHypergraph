// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : nullptr;
	PlayerController = PlayerController == nullptr ? Cast<AShooterController>(GetPawn()->GetController()) : nullptr;

	if (Character && PlayerController)
	{
		PlayerController->SetHUDKills(Score);
	}
}

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;

	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : nullptr;
	PlayerController = PlayerController == nullptr ? Cast<AShooterController>(GetPawn()->GetController()) : nullptr;

	if (Character && PlayerController)
	{
		PlayerController->SetHUDKills(Score);
	}
}
