// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"
#include "GPI/ShooterSpawnPoint.h"
#include "Kismet/GameplayStatics.h"

AActor* AShooterGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	return GetRandomSpawnPoint();
}

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

void AShooterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		// Deposesses the Pawn
		ElimmedCharacter->Reset();
		// Destroy the Pawn
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		auto SpawnPoint = GetRandomSpawnPoint();
		RestartPlayerAtPlayerStart(ElimmedController, SpawnPoint);
	}
}

AShooterSpawnPoint* AShooterGameMode::GetRandomSpawnPoint() const
{
	int spawnPointsCount = SpawnPoints.Num();

	if (spawnPointsCount <= 0)
	{
		if (DefaultSpawnPoint == nullptr)
		{
			return Cast<AShooterSpawnPoint>(UGameplayStatics::GetActorOfClass(GetWorld(), AShooterSpawnPoint::StaticClass()));
		}
		else
		{
			return DefaultSpawnPoint;
		}
	}

	int32 idx = FMath::RandRange(0, SpawnPoints.Num() - 1);

	return SpawnPoints[idx];
}

void AShooterGameMode::RegisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint)
{
	if (!SpawnPoints.AddUnique(SpawnPoint))
	{
		FString SpawnPointName = SpawnPoint ? SpawnPoint->GetName() : TEXT("NullSpawnPoint");
		UE_LOG(LogTemp, Warning, TEXT("SpawnPoint <'%s'> was already Registered."), &SpawnPointName);
	}
}

void AShooterGameMode::UnregisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint)
{
	if (!SpawnPoints.RemoveSingle(SpawnPoint))
	{
		FString SpawnPointName = SpawnPoint ? SpawnPoint->GetName() : TEXT("NullSpawnPoint");
		UE_LOG(LogTemp, Warning, TEXT("Tried to Unregister Non-Registered SpawnPoint <'%s' >"), &SpawnPointName)
	}
}
