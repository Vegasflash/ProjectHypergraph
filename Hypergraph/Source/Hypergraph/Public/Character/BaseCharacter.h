// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterCombatComponent.h"
#include "GameEnums.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Interfaces/CrosshairInteractable.h"
#include "Interfaces/ProjectileTarget.h"
#include "Interfaces/Damageable.h"
#include "Controller/ShooterController.h"
#include "ActorComponent/DamageProcessingComponent.h"
#include "BaseCharacter.generated.h"


UCLASS()
class HYPERGRAPH_API ABaseCharacter : public ACharacter , public ICrosshairInteractable, public IProjectileTarget, public IDamageable
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void Elim();

	// IProjectileTarget Inteface overrides
	virtual const ESurfaceType GetSurfaceType_Implementation() override;
	// IDamageable Inteface overrides
	UFUNCTION()
	virtual void ReceiveDamage_Implementation(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;

	UFUNCTION(Server, Reliable)
	void Server_DeactivatePlayer();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DeactivatePlayer();
	void DeactivatePlayer();

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

	virtual void OnRep_ReplicatedMovement() override;


private:
	TArray<ICharacterComponent*> CharacterComponents;
	AShooterController* ShooterController;

	// Camera 
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
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

	/* Player Health */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;
	bool bKilled = false;

	UFUNCTION()
	void OnRep_Health();
	void UpdateHUDHealth();
	//

	/* Hit Direction.*/
	EDirection LastHitDirection;

	UFUNCTION(Server, Reliable)
	void Server_PlayDamageMontage(const EDirection& HitDirection, const float& Damage);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDamageMontage(const EDirection& HitDirection, const float& Damage);
	//

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
	FORCEINLINE const bool ShouldRotateRootBone() const { return CombatComponent ? CombatComponent->ShouldRotateRootBone() : false; }
	FORCEINLINE bool IsKilled() const { return bKilled; }

	/*DEBUG*/
	FORCEINLINE const bool DebugHitScanEnabled() const { return bHitScanDebugEnabled; }
	/**/
};
