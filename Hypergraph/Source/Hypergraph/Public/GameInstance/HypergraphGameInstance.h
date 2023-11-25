// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultithreadSubsystem.h"

#include "HypergraphGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UHypergraphGameInstance : public UGameInstance
{
	GENERATED_BODY()

	TSharedPtr<UMultithreadSubsystem> MultithreadSubsystem;
	
	virtual void Init() override;
	virtual void Shutdown() override;
private:
};
