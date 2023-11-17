// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Character/BaseCharacter.h"
#include "Controller/ShooterController.h"
#include "ShooterGameMode.generated.h"

class AShooterSpawnPoint;

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	AShooterSpawnPoint* DefaultSpawnPoint;
	UPROPERTY(VisibleAnywhere)
	TArray<AShooterSpawnPoint*> SpawnPoints;
	
public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void PlayerEliminated(ABaseCharacter* ElimmedCharacter, AShooterController* VictimController, AShooterController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void RegisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint);
	void UnregisterPlayerSpawn(AShooterSpawnPoint* SpawnPoint);

	UFUNCTION(BlueprintCallable)
	AShooterSpawnPoint* GetRandomSpawnPoint() const;
};
