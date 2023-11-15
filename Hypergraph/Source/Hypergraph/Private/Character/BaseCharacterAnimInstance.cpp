// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BaseCharacterAnimInstance.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/BaseWeapon.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BaseCharacter == nullptr) // Just in case.
	{
		BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	}

	if (BaseCharacter == nullptr) return;

	bKilled = BaseCharacter->IsKilled();

	FVector Velocity = BaseCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;

	bJumpActionDetected = BaseCharacter->bPressedJump;
	bIsWeaponEquipped = BaseCharacter->IsWeaponEquipped();
	bIsCrouched = BaseCharacter->bIsCrouched;
	bIsAiming = BaseCharacter->IsAiming();
	TurningInPlace = BaseCharacter->GetTurningInPlace();
	bRotateRootBone = BaseCharacter->ShouldRotateRootBone();

	const FRotator AimRotation = BaseCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BaseCharacter->GetVelocity());
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	LastFrameCharRotation = CharRotation;
	CharRotation = BaseCharacter->GetActorRotation();
	const FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(CharRotation, LastFrameCharRotation);
	const float Target = RotationDelta.Yaw / DeltaTime;
	const float Lerp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Lerp, -90.f, 90.f);

	if (const UCharacterCombatComponent* CombatComponent = BaseCharacter->GetCharacterCombat())
	{
		AimOffset_Yaw = CombatComponent->GetAO_Yaw();
		AimOffset_Pitch = CombatComponent->GetAO_Pitch();
		EquippedWeapon = CombatComponent->GetEquippedWeapon();

		if (bIsWeaponEquipped && EquippedWeapon)
		{
			const auto CharacterMesh = BaseCharacter->GetMesh();
			const auto WeaponMesh = EquippedWeapon->GetWeaponMesh();

			if (CharacterMesh && WeaponMesh)
			{
				LeftHandTransform = WeaponMesh->GetSocketTransform(FName("Left_Hand_Socket"), RTS_World);

				FVector OutLocation;
				FRotator OutRotation;
				CharacterMesh->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(),
				                                    FRotator::ZeroRotator, OutLocation, OutRotation);
				LeftHandTransform.SetLocation(OutLocation);
				LeftHandTransform.SetRotation(FQuat(OutRotation));

				auto CrosshairImpactPoint = BaseCharacter->GetScanHitResult()->ImpactPoint;

				bLocallyControlled = BaseCharacter->IsLocallyControlled();
				if (bLocallyControlled)
				{
					const auto RightHandTransform = WeaponMesh->GetSocketTransform(FName("hand_r"), RTS_World);

					FRotator WeaponLookAtDir = UKismetMathLibrary::FindLookAtRotation(
						RightHandTransform.GetLocation(),
						RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - CrosshairImpactPoint));

					RightHandRotation = FMath::RInterpTo(RightHandRotation, WeaponLookAtDir, DeltaTime, 15.f);

					const auto MuzzleTipTransform = WeaponMesh->GetSocketTransform(FName(("MuzzleFlash")));
					FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

					if (BaseCharacter->DebugHitScanEnabled())
					{
						DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
						DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), CrosshairImpactPoint, FColor::Orange);
					}
				}
			}
		}
	}
}
