// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICharacterComponent.h"
#include "InputAction.h"
#include "GameEnums.h"
#include "HUD/ShooterHUD.h"
#include "CharacterCombatComponent.generated.h"

#define TRACE_LENGTH 10000

class ABaseCharacter;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HYPERGRAPH_API UCharacterCombatComponent : public UActorComponent, public ICharacterComponent
{
	GENERATED_BODY()

public:
	UCharacterCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class ABaseCharacter;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Input)
	void SetupInputs(UEnhancedInputComponent* PlayerInputComponent);
	virtual void SetupInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Input)
	void DisableInputs(UEnhancedInputComponent* PlayerInputComponent);
	virtual void DisableInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(ABaseWeapon* WeaponToEquip);
	void ProcessWeaponEquip();

	void OnRep_ReplicatedMovement_Implementation();

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool IsAiming);
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	const float CalculateSpeed() const;

	UFUNCTION(Server, Reliable)
	void Server_EquipButtonPressed();
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool IsAiming);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void ProcessFiring();

	void ScanUnderCrossHair(FHitResult& TraceHitResult);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AimWalkSpeed;

	bool ValidateCharacterRef() const;
	void EquipInputPressed();
	void EquipDetectedWeapon();

	void AimInputPressed();
	void AimInputReleased();
	void AimInputProcess(bool IsPressed);

	void ShootInputPressed();
	void ShootInputReleased();
	void ShootInputProcess(bool IsPressed);

	void PlayFireRifleMontage(bool bAiming);
	void PlayHitReactMontage(const EDirection& Direction);
	void PlayDeathReactMontage(const EDirection& Direction);

	UFUNCTION(BlueprintCallable)
	void OnDeathEnd();

	void SetFiringMode(EFiringMode FiringMode);


	UPROPERTY()
	class ABaseCharacter* Character;
	class APlayerController* PlayerController;
	class AShooterHUD* ShooterHUD;
	FHUDPackage HUDPackage;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class ABaseWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

	bool bIsFiring;
	float ShotIntervalCooldown;

	float AimOffset_Yaw;
	float Interp_AO_Yaw;
	float AimOffset_Pitch;
	FRotator StartingAimRot;

	// Crosshair Variables
	double CrosshairVelocityFactor;
	double CrosshairInAirFactor;
	double CrosshairFiringFactor;
	double CrosshairAimFactor;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float MaxCrosshairInAirSpread;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairSpreadInAirInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairSpreadLandedInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairSpreadRecoilReductionInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrossHairOffsetDefault = 0.5f;

	/*
	* Aiming and FoV
	*/
	float DefaultFOV; // When not aiming weapon, lerp back to this default.
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomedFoV = 30.f;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float UnZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = Zoom)
	float CrossHairZoomedOffset = 0.58f;

	void InterpFoV(float DeltaTime);
	//

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	TMap<EDirection, FName> DirectionNameForMontageMap;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* FireRifleMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DeathReactMontage;

	void SetHUDCrosshairs(float DeltaTime);

	FHitResult ScanHitResult;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication = 0;
	EFiringMode CurrentFiringMode;

public:

	FORCEINLINE float GetAO_Yaw() const { return AimOffset_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AimOffset_Pitch; }
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE ETurningInPlace GetETurningInPlace() const { return TurningInPlace; }
	FORCEINLINE float GetAimWalkSpeed() const { return AimWalkSpeed; }
	FORCEINLINE const bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE const bool ShouldRotateRootBone() const { return bRotateRootBone; }
};
