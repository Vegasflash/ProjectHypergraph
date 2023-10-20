// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICharacterComponent.h"
#include "InputAction.h"
#include "GameEnums.h"
#include "CharacterCombatComponent.generated.h"

#define TRACE_LENGTH 10000

class ABaseCharacter;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HYPERGRAPH_API UCharacterCombatComponent : public UActorComponent, public ICharacterComponent
{
	GENERATED_BODY()

public:
	UCharacterCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class ABaseCharacter;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category= Input)
	void SetupInputs(UEnhancedInputComponent* PlayerInputComponent);
	virtual void SetupInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category= Input)
	void DisableInputs(UEnhancedInputComponent* PlayerInputComponent);
	virtual void DisableInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(ABaseWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool IsAiming);
	void AimOffset(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_EquipButtonPressed();
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool IsAiming);
	UFUNCTION(Server, Reliable)
	void ServerFire();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();

	void ScanUnderCrossHair(FHitResult& TraceHitResult);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Movement, meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY()
	class ABaseCharacter* Character;
	
	UPROPERTY(ReplicatedUsing= OnRep_EquippedWeapon)
	class ABaseWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;
	
	float AimOffset_Yaw;
	float Interp_AO_Yaw;
	float AimOffset_Pitch;
	FRotator StartingAimRot;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category= "Combat")
	class UAnimMontage* FireRifleMontage;

	FHitResult HitResult;
public:

	FORCEINLINE float GetAO_Yaw() const { return AimOffset_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AimOffset_Pitch; }
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const {return EquippedWeapon; }
	FORCEINLINE ETurningInPlace GetETurningInPlace() const {return TurningInPlace; }
	FORCEINLINE float GetAimWalkSpeed() const {return AimWalkSpeed; }
	FORCEINLINE FHitResult GetHitResult() const {return HitResult; }
};
