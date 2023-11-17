// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/DeathEndAnimNotify.h"
#include "Engine.h"
#include "Character/BaseCharacter.h"

void UDeathEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		if (MeshComp->GetOwnerRole() >= ENetRole::ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server Heard Death"));
			Character->Server_DoPlayerDeathSequence();
		}
	}
}
