// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"
#include "GPI/ShooterSpawnPoint.h"
#include "Kismet/GameplayStatics.h"

void AShooterGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);


}

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

	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState = VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;
	if (AttackerPlayerState && (AttackerPlayerState != VictimPlayerState))
	{
		AttackerPlayerState->AddToScore(1.f);
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
		return Cast<AShooterSpawnPoint>(UGameplayStatics::GetActorOfClass(GetWorld(), AShooterSpawnPoint::StaticClass()));
	}

	int32 idx = FMath::RandRange(0, spawnPointsCount - 1);

	return SpawnPoints[idx];
}

void AShooterGameMode::RegisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint)
{
	if (SpawnPoints.AddUnique(SpawnPoint) == -1)
	{
		FString SpawnPointName = SpawnPoint ? SpawnPoint->GetName() : TEXT("NullSpawnPoint");
		UE_LOG(LogTemp, Warning, TEXT("SpawnPoint <'%s'> was already Registered."), &SpawnPointName);
	}
}

void AShooterGameMode::UnregisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint)
{
	if (SpawnPoints.RemoveSingle(SpawnPoint) == -1)
	{
		FString SpawnPointName = SpawnPoint ? SpawnPoint->GetName() : TEXT("NullSpawnPoint");
		UE_LOG(LogTemp, Warning, TEXT("Tried to Unregister Non-Registered SpawnPoint <'%s' >"), &SpawnPointName)
	}
}
