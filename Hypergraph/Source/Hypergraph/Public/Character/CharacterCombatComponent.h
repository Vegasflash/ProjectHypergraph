// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICharacterComponent.h"
#include "InputAction.h"
#include "CharacterCombatComponent.generated.h"

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
	void SetShooting(bool IsShooting);
	void AimOffset(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_EquipButtonPressed();
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool IsAiming);
	UFUNCTION(Server, Reliable)
	void Server_SetShooting(bool IsShooting);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	bool ValidateCharacterRef() const;
	void EquipInputPressed();
	void EquipDetectedWeapon();

	void AimInputPressed();
	void AimInputReleased();
	void AimInputProcess(bool IsPressed);

	void ShootInputPressed();
	void ShootInputReleased();
	void ShootInputProcess(bool IsPressed);

	UPROPERTY()
	class ABaseCharacter* Character;
	
	UPROPERTY(ReplicatedUsing= OnRep_EquippedWeapon)
	class ABaseWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;
	
	UPROPERTY(Replicated)
	bool bIsShooting;
	
	float AimOffset_Yaw;
	float AimOffset_Pitch;
	FRotator StartingAimRot;

public:

	FORCEINLINE float GetAO_Yaw() const { return AimOffset_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AimOffset_Pitch; }
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const {return EquippedWeapon; }
};
