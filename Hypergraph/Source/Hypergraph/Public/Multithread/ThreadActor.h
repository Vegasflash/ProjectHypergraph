// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "ThreadActor.generated.h"

#define SUSPEND_EXIT_KILL_WAIT(thread, runnable) \
        if ((thread) && (runnable).IsValid()) { \
            (thread)->Suspend(true); \
            (runnable)->Exit(); \
            (thread)->Suspend(false); \
            (thread)->Kill(); \
            (thread)->WaitForCompletion(); \
        } \

UCLASS()
class HYPERGRAPH_API AThreadActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThreadActor();

	UFUNCTION(BlueprintCallable, Category = "Calculations")
	void InitCalculations(int32 _Calcluations);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PrintCalcData();
	int32 ProcessedCalculation;

	TUniquePtr<class FActorRunnable> ActorRunnable = nullptr;

	FRunnableThread* CurrentRunningThread = nullptr;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	TQueue<int32> ThreadCalcQueue;
};
