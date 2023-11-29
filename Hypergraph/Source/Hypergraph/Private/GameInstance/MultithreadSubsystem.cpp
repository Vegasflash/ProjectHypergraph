// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/MultithreadSubsystem.h"


void UMultithreadSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GEngine)
	{
		FString ClassName = GetClass()->GetName();
		auto DebugMessage = FString::Printf(TEXT("%s Initialized"), *ClassName);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, DebugMessage);
	}

	CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	// Right off the rip lets start a runnable and check how much time it takes to execute.

	//TestRunnable = MakeShared<FGenericRunnable>();
	//if (TestRunnable.IsValid())
	//{
	//	TestRunnable->OnCompletion.BindUFunction(this, FName("CallbackTest"));
	//	TestRunnable->Begin();
	//}

}

void UMultithreadSubsystem::CallbackTest()
{
	float diff = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - CurrentTime;

	UE_LOG(LogTemp, Warning, TEXT("Time Elapsed: %f"), diff);
}

void UMultithreadSubsystem::Deinitialize()
{
	Super::Deinitialize();
	//if (TestRunnable.IsValid())
	//{
	//	TestRunnable->Stop();
	//}
}
