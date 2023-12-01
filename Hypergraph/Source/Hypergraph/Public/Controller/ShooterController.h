// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFrameWork/PlayerState.h"
#include "ShooterController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLoggedInDelegate, APlayerState*, State);

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API AShooterController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDKills(float Kills);
	void SetHUDDeaths(float Deaths);

	virtual void OnPossess(APawn* aPawn) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerLoggedInDelegate OnPlayerLoggedIn;

protected:
	virtual void BeginPlay() override;

private:
	class AShooterHUD* ShooterHUD;




	
};
