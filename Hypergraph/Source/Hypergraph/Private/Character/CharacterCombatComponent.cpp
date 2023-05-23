// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterCombatComponent.h"

UCharacterCombatComponent::UCharacterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	auto Character = Cast<ABaseCharacter>(GetOwner());
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCharacterCombatComponent::SetupInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 10, FColor::Green, TEXT("SETTING UP INPUTs."));
	}
}

void UCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

