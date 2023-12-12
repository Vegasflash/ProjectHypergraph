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
#include "Interfaces/WorldInteractor.h"
#include "Controller/ShooterController.h"
#include "ActorComponent/DamageProcessingComponent.h"
#include "Components/TimelineComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include <GameplayEffectTypes.h>

#include "BaseCharacter.generated.h"

static const FName DISSOLVE_PARAM_NAME = TEXT("Dissolve");
static const FName GLOW_INT_PARAM_NAME = TEXT("GlowIntensity");

class AShooterPlayerState;

UCLASS()
class HYPERGRAPH_API ABaseCharacter : public ACharacter , public ICrosshairInteractable, public IProjectileTarget, public IAbilitySystemInterface
{
	GENERATED_BODY()

	const float MAX_DISSOLVE_PARAM = 0.55f;
	const float MIN_DISSOLVE_PARAM = -0.25f;

public:

	// Sets default values for this character's properties
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Elim();

	//~ IProjectileTarget Interface
	virtual const ESurfaceType GetSurfaceType_Implementation() override;
	//~ End IProjectileTarget Interface
	
	// IDamageable Interface

	UFUNCTION(Server, Reliable)
	void Server_HitDetected(AController* Attacker, const EDirection& HitDirection);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HitDetected(AController* Attacker, const EDirection& HitDirection);
	virtual void HitDetected(AController* Attacker, const EDirection& HitDirection);

	virtual void KillCharacter(AController* Attacker, const EDirection& HitDirection);
	//~ End IDamageable Interface

	//~ Begin IAbilitySystemInterface
/** Returns our Ability System Component. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface

	UFUNCTION(Server, Reliable)
	void Server_DoPlayerDeathSequence();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DoPlayerDeathSequence();
	void DoPlayerDeathSequence();

	float GetHealth() const;
	float GetMaxHealth() const;

	UFUNCTION(Server, Reliable)
	void RefreshHUD_Server();
	UFUNCTION(NetMulticast, Reliable)
	void RefreshHUD_Multicast();

	void UpdateHUDHealth();
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

	mutable TWeakObjectPtr<AShooterController> ShooterController;
	AShooterController* GetShooterController() const;

	mutable TWeakObjectPtr<AShooterPlayerState> ShooterPlayerState;
	AShooterPlayerState* GetShooterPlayerState() const;

	// Camera 
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> OverheadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterCombatComponent> CombatComponent;
	
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class ABaseWeapon* OverlappingWeapon;
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon);

	void HideCameraIfCameraClose();

	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraThreshold = 200.f;

	/* Player Health */
	//UPROPERTY(EditAnywhere, Category = "Player Stats")
	//float MaxHealth = 100.f;
	//UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	//float Health = 100.f;
	bool bKilled = false;

	//UFUNCTION()
	//void OnRep_Health();
	
	//

	/* Dissolve Effect*/
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UTimelineComponent* DissolveTimeline;
	UPROPERTY(EditAnywhere, Category = "Elim")
	UCurveFloat* DissolveCurve;

	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
	void OnDissolveTimelineTick(float DissolveValue);
	void StartDissolve();

	// 
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance; // instance assigned to blueprint.
	UPROPERTY(VisibleAnywhere, Category="Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance; // Runtime Instance
	//

	/* Hit Direction.*/
	EDirection LastHitDirection;

	UFUNCTION(Server, Reliable)
	void Server_PlayDamageMontage(const EDirection& HitDirection, const float& Damage);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDamageMontage(const EDirection& HitDirection, const float& Damage);
	//

	// Elim Timer
	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();
	//

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
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
	FORCEINLINE const FVector& GetHitPosition() const { return CombatComponent ? CombatComponent->GetHitPosition() : FVector::ZeroVector; }
	FORCEINLINE UCameraComponent* GetCamera() const { return FollowCamera; }
	FORCEINLINE const bool ShouldRotateRootBone() const { return CombatComponent ? CombatComponent->ShouldRotateRootBone() : false; }
	FORCEINLINE bool IsKilled() const { return bKilled; }

	/*DEBUG*/
	FORCEINLINE const bool DebugHitScanEnabled() const { return bHitScanDebugEnabled; }
	/**/
};
