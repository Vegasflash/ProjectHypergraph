// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterCombatComponent.h"

#include "EnhancedInputComponent.h"
#include "Character/BaseCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameEnums.h"

UCharacterCombatComponent::UCharacterCombatComponent() :
	MaxCrosshairInAirSpread(2.25f),
	CrosshairSpreadInAirInterpSpeed(2.25f),
	CrosshairSpreadLandedInterpSpeed(30.f),
	CrosshairSpreadRecoilReductionInterpSpeed(5.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	AimWalkSpeed = 600.f;

	DirectionNameForMontageMap.Add(EDirection::ED_Forward, "FromFront");
	DirectionNameForMontageMap.Add(EDirection::ED_Back, "FromBack");
	DirectionNameForMontageMap.Add(EDirection::ED_Right, "FromRight");
	DirectionNameForMontageMap.Add(EDirection::ED_Left, "FromLeft");
}

void UCharacterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCharacterCombatComponent, bIsAiming);
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;

	if (auto camera = Character->GetCamera())
	{
		DefaultFOV = camera->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
}

void UCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character == nullptr) return;

	if (Character->GetLocalRole() > ENetRole::ROLE_SimulatedProxy && Character->IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f) // If server hasn't ticked in a quarter second, we'll force a refresh.
		{
			Execute_OnRep_ReplicatedMovement(this);
		}
		CalculateAO_Pitch();
	}

	if (Character->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
		ScanUnderCrossHair(ScanHitResult);
		InterpFoV(DeltaTime);
	}

	if (EquippedWeapon != nullptr)
	{
		if (ShotIntervalCooldown > 0)
		{
			ShotIntervalCooldown -= DeltaTime;
		}

		if (bIsFiring && EquippedWeapon != nullptr && CurrentFiringMode > EFiringMode::EFM_Single)
		{

			if (ShotIntervalCooldown <= 0)
			{
				ProcessFiring();
			}
		}
	}

	
}

void UCharacterCombatComponent::OnRep_ReplicatedMovement_Implementation()
{
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0;
}


void UCharacterCombatComponent::SetupInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ThisClass::EquipInputPressed);
		PlayerInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::AimInputPressed);
		PlayerInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::AimInputReleased);
		PlayerInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &UCharacterCombatComponent::ShootInputPressed);
		PlayerInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &UCharacterCombatComponent::ShootInputReleased);
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
	SetFiringMode(EquippedWeapon->GetWeaponData()->GetDefaultFiringMode());
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

#pragma region Set Firing Mode
void UCharacterCombatComponent::SetFiringMode(EFiringMode FiringMode)
{
	CurrentFiringMode = FiringMode;
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
	if (EquippedWeapon == nullptr || Character == nullptr) return;

	const bool bIsInAir = Character->GetCharacterMovement()->IsFalling();

	const float CurrentYaw = Character->GetBaseAimRotation().Yaw;
	auto Speed = CalculateSpeed();

	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;
		const FRotator CurrentAimRotation = FRotator(0.f, CurrentYaw, 0.f);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRot);
		AimOffset_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			Interp_AO_Yaw = AimOffset_Yaw;
		}
		Character->bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}

	if (Speed > 0.01f || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRot = FRotator(0.f, CurrentYaw, 0.f);
		AimOffset_Yaw = 0.f;
		Character->bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void UCharacterCombatComponent::CalculateAO_Pitch()
{
	AimOffset_Pitch = Character->GetBaseAimRotation().Pitch;

	// Unreal compresses the the Rotator's float to an int, we need to offset the difference on the Server.
	if (AimOffset_Pitch > 90.f && !Character->IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		const FVector2d InRange = FVector2d(270.f, 360.f);
		const FVector2d OutRange = FVector2d(-90.f, 0.f);
		AimOffset_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffset_Pitch);
	}
}

void UCharacterCombatComponent::SimProxiesTurn()
{
	if (EquippedWeapon == nullptr) return;

	bRotateRootBone = false;
	auto Speed = CalculateSpeed();

	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	if (Character)
	{
		ProxyRotationLastFrame = ProxyRotation;
		ProxyRotation = Character->GetActorRotation();
		ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

		if (FMath::Abs(ProxyYaw) > TurnThreshold)
		{
			if (ProxyYaw > TurnThreshold)
			{
				TurningInPlace = ETurningInPlace::ETIP_Right;
			}
			else if (ProxyYaw < -TurnThreshold)
			{
				TurningInPlace = ETurningInPlace::ETIP_Right;
			}
			else
			{
				TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			}
			return;
		}
	}

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

const float UCharacterCombatComponent::CalculateSpeed() const
{
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

#pragma endregion

void UCharacterCombatComponent::InterpFoV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bIsAiming)
	{
		const UWeaponDataAsset* WeaponData = EquippedWeapon->GetWeaponData();
		CurrentFOV = FMath::FInterpTo(CurrentFOV, WeaponData->GetZoomedFoV(), DeltaTime, WeaponData->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, UnZoomInterpSpeed);
	}

	if (Character)
	{
		if (auto Camera = Character->GetCamera())
		{
			Camera->SetFieldOfView(CurrentFOV);
		}
	}
}

void UCharacterCombatComponent::TurnInPlace(float DeltaTime)
{
	if (AimOffset_Yaw > 90.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;

	}
	else if (AimOffset_Yaw < -90.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		Interp_AO_Yaw = FMath::FInterpTo(Interp_AO_Yaw, 0, DeltaTime, 4.0f);
		AimOffset_Yaw = Interp_AO_Yaw;
		if (FMath::Abs(AimOffset_Yaw) < 15.0f)
		{
			const float CurrentYaw = Character->GetBaseAimRotation().Yaw;
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRot = FRotator(0.f, CurrentYaw, 0.f);
		}
	}
}

void UCharacterCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character != nullptr && Character->Controller != nullptr)
	{
		PlayerController = PlayerController == nullptr ? Cast<APlayerController>(Character->Controller) : PlayerController;
		if (PlayerController)
		{
			ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(PlayerController->GetHUD()) : ShooterHUD;

			if (ShooterHUD == nullptr)
			{
				PlayerController->SetPause(true);
				return;
			}

			if (EquippedWeapon != nullptr)
			{
				for (auto& Elem : EquippedWeapon->GetCrosshairTextures())
				{
					if (HUDPackage.CrosshairTextureMap.Contains(Elem.Key))
					{
						HUDPackage.CrosshairTextureMap[Elem.Key] = ShooterHUD ? Elem.Value : nullptr;

					}
					else
					{
						HUDPackage.CrosshairTextureMap.Add(Elem.Key, ShooterHUD ? Elem.Value : nullptr);
					}
				}
			}
			else
			{
				HUDPackage.CrosshairTextureMap.Empty();
			}

			// Calculate Crosshair Spread
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			auto Velocity = Character->GetVelocity();
			Velocity.Z = 0;

			if (EquippedWeapon != nullptr)
			{
				/*
				* Crosshair Character Movement Offset
				*/
				CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

				/*
				* Crosshair Mid-Air Offset
				*/
				if (Character->GetCharacterMovement()->IsFalling())
				{
					CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, MaxCrosshairInAirSpread, DeltaTime, CrosshairSpreadInAirInterpSpeed);
				}
				else
				{
					CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0, DeltaTime, CrosshairSpreadLandedInterpSpeed);
				}

				/*
				* Crosshair Firing Offset
				*/
				CrosshairFiringFactor = FMath::FInterpTo(CrosshairFiringFactor, 0, DeltaTime, CrosshairSpreadRecoilReductionInterpSpeed);


				/*
				* Crosshair Aiming Offset
				*/
				if (bIsAiming)
				{
					CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, CrossHairZoomedOffset, DeltaTime, EquippedWeapon->GetWeaponData()->GetZoomInterpSpeed());
				}
				else
				{
					CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, CrossHairOffsetDefault, DeltaTime, UnZoomInterpSpeed);
				}
			}

			HUDPackage.CrosshairSpread =
				CrossHairOffsetDefault +
				CrosshairVelocityFactor +
				CrosshairInAirFactor +
				CrosshairFiringFactor -
				CrosshairAimFactor;

			ShooterHUD->SetHUDPackage(HUDPackage);
		}
	}
}

#pragma region Shoot Weapon
void UCharacterCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCharacterCombatComponent::ShootInputPressed() { ShootInputProcess(true); }

void UCharacterCombatComponent::ShootInputReleased() { ShootInputProcess(false); }

void UCharacterCombatComponent::ShootInputProcess(bool IsPressed)
{
	if (!ValidateCharacterRef()) return;
	if (EquippedWeapon == nullptr) return;

	bIsFiring = IsPressed;

	if (bIsFiring && ShotIntervalCooldown <= 0)
	{
		ProcessFiring();
	}
}

void UCharacterCombatComponent::ProcessFiring()
{
	ShotIntervalCooldown = EquippedWeapon->GetWeaponData()->GetTimeBetweenShotsInterval();

	FHitResult HitResult;
	ScanUnderCrossHair(HitResult);
	ServerFire(HitResult.ImpactPoint);
}

void UCharacterCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCharacterCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon)
	{
		PlayFireRifleMontage(true);
		EquippedWeapon->Fire(TraceHitTarget);
		CrosshairFiringFactor += EquippedWeapon->GetRecoilIntensity();
	}
}

void UCharacterCombatComponent::PlayFireRifleMontage(bool bAiming)
{
	if (EquippedWeapon == nullptr && Character != nullptr) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && FireRifleMontage)
	{
		AnimInstance->Montage_Play(FireRifleMontage);
	}
}

void UCharacterCombatComponent::PlayHitReactMontage(const EDirection& Direction)
{
	if (EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || HitReactMontage == nullptr) return;

	AnimInstance->Montage_Play(HitReactMontage);
	AnimInstance->Montage_JumpToSection(DirectionNameForMontageMap[Direction]);
}

void UCharacterCombatComponent::PlayDeathReactMontage(const EDirection& Direction)
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || HitReactMontage == nullptr) return;

	AnimInstance->Montage_Play(DeathReactMontage);
	AnimInstance->Montage_JumpToSection(DirectionNameForMontageMap[Direction]);
}

void UCharacterCombatComponent::OnDeathEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("DEATH REGISTERED."));
}



#pragma endregion

void UCharacterCombatComponent::ScanUnderCrossHair(FHitResult& TraceHitResult)
{
	FVector2d ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrossHairLoc(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f);
	FVector CrossHairWorldPos, CrossHairWorldDir;

	auto bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrossHairLoc,
		CrossHairWorldPos,
		CrossHairWorldDir
	);

	if (bScreenToWorld)
	{
		// We need to move the Start to the fron fo the character instead of in front of the camera, to make sure we are not tracing things betweeen character and camera.

		float DistanceToCharacter = (Character->GetActorLocation() - CrossHairWorldPos).Size();

		FVector Offset = CrossHairWorldDir * (DistanceToCharacter + 100.f);
		FVector Start = CrossHairWorldPos + Offset;

		if (Character->DebugHitScanEnabled())
		{
			DrawDebugSphere(GetWorld(), Start, 16.f, 12, FColor::Red, false);
		}

		FVector End = Start + CrossHairWorldDir * TRACE_LENGTH;

		FCollisionResponseParams ResponseParams;
		//ResponseParams.CollisionResponse.

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility, FCollisionQueryParams::DefaultQueryParam, ResponseParams);

		HUDPackage.CrosshairColor = FLinearColor::White;
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		else if (auto actorHit = TraceHitResult.GetActor())
		{
			HUDPackage.CrosshairColor = actorHit->Implements<UCrosshairInteractable>() ? FLinearColor::Red : FLinearColor::White;
		}
	}
}
