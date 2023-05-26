// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterCombatComponent.h"

#include "EnhancedInputComponent.h"
#include "Character/BaseCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/BaseWeapon.h"

UCharacterCombatComponent::UCharacterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AimOffset(DeltaTime);
}

void UCharacterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCharacterCombatComponent, bIsAiming);
}

void UCharacterCombatComponent::SetupInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ThisClass::EquipInputPressed);
		PlayerInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::AimInputPressed);
		PlayerInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::AimInputReleased);
	}
}

void UCharacterCombatComponent::DisableInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent)
{
	ICharacterComponent::DisableInputs_Implementation(PlayerInputComponent);
}

bool UCharacterCombatComponent::ValidateCharacterRef() const
{
	const bool hasRef = Character != nullptr;
	if (!hasRef)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 10, FColor::Cyan, FString::Printf(TEXT("Character is null...")));
		}
	}

	return hasRef;
}

#pragma region Equip Weapon

void UCharacterCombatComponent::EquipWeapon(ABaseWeapon* WeaponToEquip)
{
	if (!ValidateCharacterRef() || WeaponToEquip == nullptr) return;

	const FName SocketName = FName("Weapon_Socket_R");
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	if (const auto WeaponSocket = Character->GetMesh()->GetSocketByName(SocketName))
	{
		WeaponSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 10, FColor::Cyan,
			                                 FString::Printf(
				                                 TEXT("Could not find SocketName: %s"), *SocketName.ToString()));
		}
	}

	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCharacterCombatComponent::Server_EquipButtonPressed_Implementation()
{
	EquipDetectedWeapon();
}

// When input detected.
void UCharacterCombatComponent::EquipInputPressed()
{
	if (!ValidateCharacterRef()) return;

	if (Character->HasAuthority())
	{
		EquipDetectedWeapon();
	}
	else
	{
		Server_EquipButtonPressed();
	}
}

void UCharacterCombatComponent::EquipDetectedWeapon()
{
	if (Character)
	{
		EquipWeapon(Character->GetOverlappingWeapon());
	}
}

#pragma endregion

#pragma region Aim Weapon

void UCharacterCombatComponent::SetAiming(bool IsAiming)
{
	bIsAiming = IsAiming;
	Server_SetAiming(IsAiming);
}

void UCharacterCombatComponent::Server_SetAiming_Implementation(bool IsAiming)
{
	bIsAiming = IsAiming;
	
}

void UCharacterCombatComponent::AimInputPressed() { AimInputProcess(true); }

void UCharacterCombatComponent::AimInputReleased() { AimInputProcess(false); }

void UCharacterCombatComponent::AimInputProcess(bool IsPressed)
{
	if (!ValidateCharacterRef()) return;
	
	SetAiming(IsPressed);
}

void UCharacterCombatComponent::AimOffset(float DeltaTime)
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;
	
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Size();
	const bool bIsInAir = Character->GetCharacterMovement()->IsFalling();

	const float CurrentYaw = Character->GetBaseAimRotation().Yaw;
	if(Speed == 0.1f && !bIsInAir)
	{
		const FRotator CurrentAimRotation = FRotator(0.f, CurrentYaw, 0.f);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRot);
		AimOffset_Yaw = DeltaAimRotation.Yaw;
		Character->bUseControllerRotationYaw = false;
	}
	
	if(Speed > 0.1f || bIsInAir)
	{
		StartingAimRot = FRotator(0.f, CurrentYaw, 0.f);
		AimOffset_Yaw = 0.f;
		Character->bUseControllerRotationYaw = true;
	}

	AimOffset_Pitch = Character->GetBaseAimRotation().Pitch;
	
	// Unreal compresses the the Rotator's float to an int, we need to offset the difference on the Server.
	if(AimOffset_Pitch > 90.f && !Character->IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		const FVector2d InRange = FVector2d(270.f, 360.f);
		const FVector2d OutRange = FVector2d(-90.f, 0.f);
		AimOffset_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffset_Pitch);
	}
}

#pragma endregion

#pragma region Shoot Weapon
void UCharacterCombatComponent::SetShooting(bool IsShooting)
{
	bIsShooting = IsShooting;
	Server_SetShooting(IsShooting);
}

void UCharacterCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCharacterCombatComponent::Server_SetShooting_Implementation(bool IsShooting)
{
	bIsShooting = IsShooting;
}

void UCharacterCombatComponent::ShootInputPressed() { ShootInputProcess(true); }

void UCharacterCombatComponent::ShootInputReleased() { ShootInputProcess(false); }

void UCharacterCombatComponent::ShootInputProcess(bool IsPressed)
{
	if (!ValidateCharacterRef()) return;
	
	Server_SetShooting(IsPressed);
}
#pragma endregion
