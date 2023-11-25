// Fill out your copyright notice in the Description page of Project Settings.

#include "Multithread/GenericRunnable.h"

#pragma region Main Thread Code 
// This code will be run on the thread that invoked this thread (i.e. game thread)

FGenericRunnable::FGenericRunnable()
{
	
}

FGenericRunnable::~FGenericRunnable()
{
	if (Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
}
#pragma endregion
// The code below will run on the new thread.

void FGenericRunnable::Begin()
{
	Thread = FRunnableThread::Create(this, TEXT("Test Thread"));
}

bool FGenericRunnable::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("My Worker Thread has been initialized"));

	return true;
}

uint32 FGenericRunnable::Run()
{
	bStopThread = false;

	// Keep processing until we're cancelled through Stop() or we're done,
	// although this thread will suspended for other stuff to happen at the same time
	while (!bStopThread)
	{
		// This is where we would do our expensive threaded processing

		// Instead we're going to make a really busy while loop to slow down processing
		// You can change INT_MAX to something smaller if you want it to run faster
		int32 x = 0;
		while (x < INT_MAX)
		{
			x++;
		}

		FPlatformProcess::Sleep(1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Worker finished counting from 0 to max"));
		break;
	}

	OnCompletion.ExecuteIfBound();

	return 0;
}

void FGenericRunnable::Exit()
{
}

// This function is NOT run on the new thread!
void FGenericRunnable::Stop()
{
	// Clean up memory usage here, and make sure the Run() function stops soon
	// The main thread will be stopped until this finishes!
	bStopThread = true;
}



