// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Multithread/GenericRunnable.h"

#include "MultithreadSubsystem.generated.h"

UCLASS()
class HYPERGRAPH_API UMultithreadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	TSharedPtr<FGenericRunnable> TestRunnable;
	float CurrentTime;

	UFUNCTION()
	void CallbackTest();

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
