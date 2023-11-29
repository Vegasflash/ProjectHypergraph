// Fill out your copyright notice in the Description page of Project Settings.


#include "Multithread/ThreadActor.h"
#include "Multithread/ActorRunnable.h"

// Sets default values
AThreadActor::AThreadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AThreadActor::InitCalculations(int32 _Calcluations)
{
	if (_Calcluations > 0)
	{
		if (!ActorRunnable.IsValid())
		{
			ActorRunnable = MakeUnique<FActorRunnable>(_Calcluations, this);
		}

		if (CurrentRunningThread == nullptr)
		{
			CurrentRunningThread = FRunnableThread::Create(ActorRunnable.Get(), TEXT("Calculation Thread"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Calculations must be greater than 0."));
	}
}

// Called when the game starts or when spawned
void AThreadActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AThreadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintCalcData();
}

void AThreadActor::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	SUSPEND_EXIT_KILL_WAIT(CurrentRunningThread, ActorRunnable)
}

void AThreadActor::PrintCalcData()
{
	if (!ThreadCalcQueue.IsEmpty() && ThreadCalcQueue.Dequeue(ProcessedCalculation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Processed Calculation: %d"), ProcessedCalculation);
	}
}