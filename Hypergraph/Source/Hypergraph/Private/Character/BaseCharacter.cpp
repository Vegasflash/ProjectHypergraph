// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Character/CharacterCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Hypergraph.h"
#include "Weapon/BaseWeapon.h"
#include "GameEnums.h"
#include "GameMode/ShooterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/ShooterPlayerState.h"
#include "TimerManager.h"
#include "GAS//GlobalAttributeSet.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	FollowCamera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	if (UCharacterMovementComponent* CharacterMvmnt = GetCharacterMovement())
	{
		CharacterMvmnt->bOrientRotationToMovement = true;
		CharacterMvmnt->NavAgentProps.bCanCrouch = true;
	}

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Display"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCharacterCombatComponent>(TEXT("Combat Component"));
	CombatComponent->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	NetUpdateFrequency = 66;
	MinNetUpdateFrequency = 33;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

#pragma region Character Defaults
void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}

	for (const auto Component : GetComponents())
	{
		if (!Component->GetClass()->ImplementsInterface(UCharacterComponent::StaticClass())) continue;

		if (ICharacterComponent* CharComponent = Cast<ICharacterComponent>(Component))
		{
			CharacterComponents.Add(CharComponent);
		}
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	RefreshHUD_Server();
}


void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideCameraIfCameraClose();
}

#pragma endregion

void ABaseCharacter::Elim()
{
	Multicast_Elim();
	GetWorld()->GetTimerManager().SetTimer(
		ElimTimer,
		this,
		&ThisClass::ElimTimerFinished,
		ElimDelay);
}

void ABaseCharacter::ElimTimerFinished()
{
	if (AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>())
	{
		GameMode->RequestRespawn(this, Controller);
	}
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (auto ShooterState = Cast<AShooterPlayerState>(GetPlayerState()))
	{
		ShooterState->OnShooterCharacterPossesed(this);
	}
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!GetShooterPlayerState()) return;

	ShooterPlayerState->OnShooterCharacterStateReplicated(this);

	if (!GetShooterController()) return;

	ShooterController->SetHUDHealth(GetHealth(), GetMaxHealth());
	ShooterController->SetHUDKills(ShooterPlayerState->GetScore());
	ShooterController->SetHUDDeaths(0);
}

void ABaseCharacter::Multicast_Elim_Implementation()
{
	bKilled = true;

	// Character died. We want to disable Pawn's inputs while they're in a death animation.
	APlayerController* LocalPlayerController = Cast<APlayerController>(GetOwner());

	if (LocalPlayerController && LocalPlayerController->IsLocalController())
	{
		if (auto Pawn = LocalPlayerController->GetPawn())
		{
			Pawn->DisableInput(LocalPlayerController);
		}
	}
}

#pragma region Interfaces


const ESurfaceType ABaseCharacter::GetSurfaceType_Implementation()
{
	return ESurfaceType::Flesh;
}

#pragma endregion

#pragma region Custom Character Movement

#pragma region Character Jump
void ABaseCharacter::JumpStartActionCallback()
{
	// Use Basic Jumps for now.
	ACharacter::Jump();
}

void ABaseCharacter::JumpEndActionCallback()
{
	// Use basic Jumps for now.
	ACharacter::StopJumping();
}
#pragma endregion

#pragma region Character Crouch
void ABaseCharacter::CrouchStartActionCallback()
{
	ACharacter::Crouch();
}

void ABaseCharacter::CrouchEndActionCallback()
{
	ACharacter::UnCrouch();
}
#pragma endregion

void ABaseCharacter::MoveActionCallback(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABaseCharacter::LookActionCallback(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABaseCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABaseCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	for (auto& component : CharacterComponents)
	{
		component->Execute_OnRep_ReplicatedMovement(component->_getUObject());
	}
}

// Getter Property.
AShooterController* ABaseCharacter::GetShooterController() const
{
	ShooterController = !ShooterController.IsValid() ? MakeWeakObjectPtr<AShooterController>(Cast<AShooterController>(GetController())) : ShooterController;
	return ShooterController.Get();
}

// Getter Property.
AShooterPlayerState* ABaseCharacter::GetShooterPlayerState() const
{
	ShooterPlayerState = !ShooterPlayerState.IsValid() ? MakeWeakObjectPtr<AShooterPlayerState>(Cast<AShooterPlayerState>(GetPlayerState())) : ShooterPlayerState;
	return ShooterPlayerState.Get();
}

#pragma endregion 

#pragma region Weapon Detection

void ABaseCharacter::SetOverlappingWeapon(ABaseWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABaseCharacter::OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	else if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABaseCharacter::HideCameraIfCameraClose()
{
	if (!IsLocallyControlled()) return;

	bool HidePlayer = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() > CameraThreshold;

	GetMesh()->SetVisibility(HidePlayer);
	if (CombatComponent)
	{
		if (auto Weapon = CombatComponent->GetEquippedWeapon())
		{
			Weapon->GetWeaponMesh()->SetOwnerNoSee(!HidePlayer);
		}
	}
}

void ABaseCharacter::Server_HitDetected_Implementation(AController* Attacker, const EDirection& HitDirection)
{
	auto curHp = GetShooterPlayerState()->GetAttributes()->GetHealth();
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("SERVER: Health = %f"), curHp);
		HitDetected(Attacker, HitDirection);
	}

	Multicast_HitDetected(Attacker, HitDirection);
}

void ABaseCharacter::Multicast_HitDetected_Implementation(AController* Attacker, const EDirection& HitDirection)
{
	auto curHp = GetShooterPlayerState()->GetAttributes()->GetHealth();
	
	UE_LOG(LogTemp, Warning, TEXT("CLIENT: Health = %f"), curHp);
	
	HitDetected(Attacker, HitDirection);
}

void ABaseCharacter::HitDetected(AController* Attacker, const EDirection& HitDirection)
{
	auto curHp = GetShooterPlayerState()->GetAttributes()->GetHealth();

	if (GetShooterPlayerState()->GetAttributes()->GetHealth() <= 0)
	{
		KillCharacter(Attacker, HitDirection);
		return;
	}

	CombatComponent->PlayHitReactMontage(HitDirection);
}

void ABaseCharacter::KillCharacter(AController* Attacker, const EDirection& HitDirection)
{
	CombatComponent->PlayDeathReactMontage(HitDirection);
	// This is only being executed on the Server

	//float HealthPreview = FMath::Clamp(GetHealth() - Damage, 0.f, GetMaxHealth());

	if (CombatComponent)
	{
		// Notify Hit Actors of impact.
		//if (DamagedActor->Implements<UProjectileTarget>())
		{
			//TScriptInterface<IProjectileTarget> target;
			//target.SetObject(DamagedActor);
			//
			//
			//ESurfaceType HitSurface = target->Execute_GetSurfaceType(DamagedActor);
		}
		//else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Projectile hit an unknown surface type."));
		}

		// Health is updated, now we execute the Animations for character hit.
		//FVector StartLocation = DamageCauser->GetActorLocation();
		//FVector EndLocation = DamagedActor->GetActorLocation();
		//
		//FCollisionQueryParams CollisionParams;
		//CollisionParams.AddIgnoredActor(DamageCauser);

		//FHitResult HitResult;
		//if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
		//{
		//	FVector ImpactNormal = HitResult.ImpactNormal;
		//
		//	FTransform Transform = DamagedActor->GetActorTransform();
		//	FVector OtherForward = Transform.GetUnitAxis(EAxis::X);
		//	FVector OtherRight = Transform.GetUnitAxis(EAxis::Y);
		//
		//	float DotForward = FVector::DotProduct(ImpactNormal, OtherForward);
		//	float DotRight = FVector::DotProduct(ImpactNormal, OtherRight);
		//
		//	LastHitDirection =
		//		DotForward > 0.7f ? LastHitDirection = EDirection::ED_Forward :
		//		DotForward < -0.7f ? LastHitDirection = EDirection::ED_Back :
		//		DotRight > 0.7f ? LastHitDirection = EDirection::ED_Right :
		//		DotRight < -0.7f ? LastHitDirection = EDirection::ED_Left :
		//		EDirection::ED_Forward;
		//
		//	if (HealthPreview <= 0)
		//	{
		//		if (AShooterGameMode* CurrentGameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
		//		{
		//			AShooterController* AttackerController = Cast<AShooterController>(InstigatedBy);
		//			CurrentGameMode->PlayerEliminated(this, GetShooterController(), AttackerController);
		//		}
		//	}
		//
		//	Server_PlayDamageMontage(LastHitDirection, Damage);
		//}
	}

	if (AShooterGameMode* CurrentGameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		AShooterController* AttackerController = Cast<AShooterController>(Attacker);
		CurrentGameMode->PlayerEliminated(this, GetShooterController(), AttackerController);
	}

	//UpdateHUDHealth();
}

#pragma region Ragdoll
void ABaseCharacter::Server_DoPlayerDeathSequence_Implementation()
{
	DoPlayerDeathSequence(); // Server
	Multicast_DoPlayerDeathSequence(); // Clients
}

void ABaseCharacter::Multicast_DoPlayerDeathSequence_Implementation()
{
	DoPlayerDeathSequence();
}

void ABaseCharacter::DoPlayerDeathSequence()
{
	if (CombatComponent && CombatComponent->EquippedWeapon)
	{
		CombatComponent->EquippedWeapon->Dropped();
	}

	if (auto Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (auto MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		MeshComp->SetAllBodiesSimulatePhysics(true);
		MeshComp->SetSimulatePhysics(true);
		MeshComp->WakeAllRigidBodies();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Mesh on this Character."));
	}

	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		GetMesh()->SetMaterial(1, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(DISSOLVE_PARAM_NAME, MAX_DISSOLVE_PARAM);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(GLOW_INT_PARAM_NAME, 200.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Dissolve Mat Instance on this Character."));
	}

	StartDissolve();
}
UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return GetShooterPlayerState() == nullptr ? nullptr : ShooterPlayerState->GetAbilitySystemComponent();
}
#pragma endregion
// It's important that we pass along the damage because the HitDirection and Health are not replicated at the same time.
void ABaseCharacter::Server_PlayDamageMontage_Implementation(const EDirection& HitDirection, const float& Damage)
{
	Multicast_PlayDamageMontage(HitDirection, Damage);
}

void ABaseCharacter::Multicast_PlayDamageMontage_Implementation(const EDirection& HitDirection, const float& Damage)
{
	if ((GetHealth() - Damage) <= 0)
	{
		CombatComponent->PlayDeathReactMontage(HitDirection);

	}
	else
	{
		CombatComponent->PlayHitReactMontage(HitDirection);
	}
}

float ABaseCharacter::GetHealth() const
{
	if (!GetShooterPlayerState()) { return 0.f; }

	if (auto attributes = ShooterPlayerState->GetAttributes())
	{
		return attributes->GetHealth();
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to find Attributes in Player State."));
	return 0.0f;
}

float ABaseCharacter::GetMaxHealth() const
{
	if (!GetShooterPlayerState()) { return 0.f; }

	if (auto attributes = ShooterPlayerState->GetAttributes())
	{
		return attributes->GetMaxHealth();

	}

	UE_LOG(LogTemp, Error, TEXT("Failed to find Attributes in Player State."));
	return 0.0f;
}

void ABaseCharacter::RefreshHUD_Server_Implementation()
{
	UpdateHUDHealth();
	RefreshHUD_Multicast();
}

void ABaseCharacter::RefreshHUD_Multicast_Implementation()
{
	UpdateHUDHealth();
}

void ABaseCharacter::UpdateHUDHealth()
{
	if (!GetShooterController()) return; 

	ShooterController->SetHUDHealth(GetHealth(), GetMaxHealth());
}

#pragma region Dissolve
void ABaseCharacter::OnDissolveTimelineTick(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(DISSOLVE_PARAM_NAME, DissolveValue);
	}
}

void ABaseCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::OnDissolveTimelineTick);

	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}
#pragma endregion

void ABaseCharacter::DebugHitScan()
{
	bHitScanDebugEnabled = !bHitScanDebugEnabled;
}

#pragma endregion 

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this,
			&ThisClass::JumpStartActionCallback);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
			&ThisClass::JumpEndActionCallback);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this,
			&ThisClass::CrouchStartActionCallback);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this,
			&ThisClass::CrouchEndActionCallback);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveActionCallback);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::LookActionCallback);

		for (auto& component : CharacterComponents)
		{
			component->Execute_SetupInputs(component->_getUObject(), EnhancedInputComponent);
		}
	}
}