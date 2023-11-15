// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Character/BaseCharacter.h"
#include "Controller/ShooterController.h"
#include "ShooterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(ABaseCharacter* EllimmedCharacter, AShooterController* VictimController, AShooterController* AttackerController);
};
