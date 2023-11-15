// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"

void AShooterGameMode::PlayerEliminated(ABaseCharacter* EllimmedCharacter, AShooterController* VictimController, AShooterController* AttackerController)
{
	// This is only being executed on the Server
	if (AttackerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't identify attacker, Null Pointer."));
	}

	if (EllimmedCharacter)
	{
		EllimmedCharacter->Elim();
	}
}
