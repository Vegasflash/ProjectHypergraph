// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/DamageProcessingComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UDamageProcessingComponent::UDamageProcessingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageProcessingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDamageProcessingComponent::ProcessDamage(AActor* DamagedActor)
{
	UGameplayStatics::ApplyDamage(DamagedActor, BaseDamage, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
}

