// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/ShooterGameMode.h"
#include "ShooterSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API AShooterSpawnPoint : public APlayerStart
{
	GENERATED_BODY()
	
private:

	AShooterGameMode* ShooterGameMode;

	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
