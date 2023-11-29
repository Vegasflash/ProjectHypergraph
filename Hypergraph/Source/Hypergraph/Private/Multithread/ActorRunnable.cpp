// Fill out your copyright notice in the Description page of Project Settings.


#include "Multithread/ActorRunnable.h"

FActorRunnable::FActorRunnable(int32 _Calculations, AThreadActor* Actor)
{
	if (_Calculations > 0 && Actor)
	{
		Calculations = _Calculations;
		CurrentThreadActor = Actor;
	}
}

FActorRunnable::~FActorRunnable()
{
}

bool FActorRunnable::Init()
{
	FRunnable::Init();
	bStopThread = false;
	CalcCount = 0;

	return true;
}

uint32 FActorRunnable::Run()
{
	while (!bStopThread)
	{
		if (CalcCount < Calculations)
		{
			CurrentCalculation += FMath::RandRange(20, 400);
			CurrentCalculation *= FMath::RandRange(2, 500);
			CurrentCalculation -= FMath::RandRange(10, 500);

			CurrentThreadActor->ThreadCalcQueue.Enqueue(CurrentCalculation);

			CalcCount++;
		}
		else
		{
			bStopThread = true;
		}
	}

	return 0;
}

void FActorRunnable::Stop()
{
	FRunnable::Stop();
}

void FActorRunnable::Exit()
{
	FGenericRunnable::Exit();

	bStopThread = true;
}
