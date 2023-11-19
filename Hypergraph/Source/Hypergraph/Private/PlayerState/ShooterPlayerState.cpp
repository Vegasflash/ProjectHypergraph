// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"

void AShooterPlayerState::PostInitializeComponents()
{
	Character = Cast<ABaseCharacter>(GetPawn());
	PlayerController = Cast<AShooterController>(GetPawn()->GetController());
}

void AShooterPlayerState::OnRep_Score()
{
	if (Character && PlayerController)
	{
		PlayerController->SetHUDKills(Score);
	}
}

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;

	if (Character && PlayerController)
	{
		PlayerController->SetHUDKills(ScoreAmount);
	}
}
