// Fill out your copyright notice in the Description page of Project Settings.


#include "Multithread/ThreadActor.h"

// Sets default values
AThreadActor::AThreadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThreadActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThreadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

