// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameEnums.h"
#include "BaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	void OnJumpApexReached();

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = "true"))
	class ABaseCharacter* BaseCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bKilled;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsCrouched;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bJumpActionDetected;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	bool bIsWeaponEquipped;

	class ABaseWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	float YawOffset;
	
	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	float Lean;

	FRotator LastFrameCharRotation;
	FRotator CharRotation;
	FRotator DeltaRotation;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	float AimOffset_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	float AimOffset_Pitch;
	
	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess = "true"))
	ETurningInPlace TurningInPlace;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	bool bLocallyControlled;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	bool bRotateRootBone;
};
