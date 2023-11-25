// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/HypergraphGameInstance.h"
#include "Subsystems/SubsystemCollection.h"

void UHypergraphGameInstance::Init()
{
    MultithreadSubsystem = TSharedPtr<UMultithreadSubsystem>(
        NewObject<UMultithreadSubsystem>(this),
        [](UMultithreadSubsystem* SubsystemToDelete)
        {
            SubsystemToDelete->ConditionalBeginDestroy();
        }
    );

	FSubsystemCollection<UMultithreadSubsystem> Collection;
	MultithreadSubsystem->Initialize(Collection);
}

void UHypergraphGameInstance::Shutdown()
{
    // Stop execution on all threads.
    if (MultithreadSubsystem.IsValid())
    {
        MultithreadSubsystem->Deinitialize();
        MultithreadSubsystem.Reset();
    }
}
