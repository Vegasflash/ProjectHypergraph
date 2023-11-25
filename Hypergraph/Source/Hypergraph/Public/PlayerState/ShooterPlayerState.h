// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Controller/ShooterController.h"
#include "Character/BaseCharacter.h"
#include "ShooterPlayerState.generated.h"

/**
 *
 */
UCLASS()
class HYPERGRAPH_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

private:

	AShooterController* PlayerController;
	ABaseCharacter* Character;

public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);


};
