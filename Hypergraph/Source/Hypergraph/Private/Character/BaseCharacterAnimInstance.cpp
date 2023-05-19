// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacterAnimInstance.h"

#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(BaseCharacter == nullptr) // Just in case.
	{
		BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	}
	
	if(BaseCharacter == nullptr) return;
	
	FVector Velocity = BaseCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
	
	bJumpActionDetected = BaseCharacter->bPressedJump;
}
