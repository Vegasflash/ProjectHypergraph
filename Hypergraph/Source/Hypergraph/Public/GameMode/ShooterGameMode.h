// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Character/BaseCharacter.h"
#include "Controller/ShooterController.h"
#include "PlayerState/ShooterPlayerState.h"
#include "ShooterGameMode.generated.h"

class AShooterSpawnPoint;

/**
 * 
 */
UCLASS(Blueprintable)
class HYPERGRAPH_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AShooterSpawnPoint>> SpawnPoints;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void PlayerEliminated(ABaseCharacter* ElimmedCharacter, AShooterController* VictimController, AShooterController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void RegisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint);
	void UnregisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint);

	UFUNCTION(BlueprintCallable)
	AShooterSpawnPoint* GetRandomSpawnPoint() const;
};
