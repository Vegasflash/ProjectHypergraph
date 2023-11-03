// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterCombatComponent.h"
#include "GameEnums.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Interfaces/CrosshairInteractable.h"
#include "BaseCharacter.generated.h"

UCLASS()
class HYPERGRAPH_API ABaseCharacter : public ACharacter , public ICrosshairInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void JumpStartActionCallback();
	virtual void JumpEndActionCallback();
	
	virtual void CrouchStartActionCallback();
	virtual void CrouchEndActionCallback();
	
	virtual void MoveActionCallback(const FInputActionValue& Value);
	virtual void LookActionCallback(const FInputActionValue& Value);

	virtual void Jump() override;

private:
	// Camera 
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	// Input 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UCharacterCombatComponent* CombatComponent;
	
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class ABaseWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon);

	void HideCameraIfCameraClose();

	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraThreshold = 200.f;
	/*DEBUG*/
	UFUNCTION(Exec, Category = "Commands")
	void DebugHitScan();
	bool bHitScanDebugEnabled = false;
	/**/

public:
	void SetOverlappingWeapon(ABaseWeapon* Weapon);
	FORCEINLINE ABaseWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	FORCEINLINE bool IsWeaponEquipped() const { return CombatComponent && CombatComponent->EquippedWeapon; }
	FORCEINLINE bool IsAiming() const { return CombatComponent && CombatComponent->bIsAiming; }
	FORCEINLINE UCharacterCombatComponent* GetCharacterCombat() const { return CombatComponent; }
	FORCEINLINE ETurningInPlace GetTurningInPlace () const {return CombatComponent ? CombatComponent->GetETurningInPlace() : ETurningInPlace::ETIP_MAX; }
	FORCEINLINE FHitResult* GetScanHitResult() const { return CombatComponent ? &CombatComponent->ScanHitResult : nullptr; }
	FORCEINLINE UCameraComponent* GetCamera() const { return FollowCamera; }

	/*DEBUG*/
	FORCEINLINE const bool DebugHitScanEnabled() const { return bHitScanDebugEnabled; }
	/**/
};
