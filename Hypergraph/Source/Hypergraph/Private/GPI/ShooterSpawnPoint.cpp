// Fill out your copyright notice in the Description page of Project Settings.


#include "GPI/ShooterSpawnPoint.h"

void AShooterSpawnPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ShooterGameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
	if (ShooterGameMode)
	{
		ShooterGameMode->RegisterPlayerSpawn(this);
	}
}

void AShooterSpawnPoint::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ShooterGameMode)
	{
		ShooterGameMode->UnregisterPlayerSpawn(this);
	}
}
