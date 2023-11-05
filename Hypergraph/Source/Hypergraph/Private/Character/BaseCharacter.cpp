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
#include "Weapon/BaseWeapon.h"

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
	if(UCharacterMovementComponent* CharacterMvmnt = GetCharacterMovement())
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
	GetCharacterMovement()->RotationRate = FRotator(0.f,0.f, 850.f);

	NetUpdateFrequency = 66;
	MinNetUpdateFrequency = 33;
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
	if(CombatComponent)
	{
		CombatComponent->Character = this;
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
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideCameraIfCameraClose();
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
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

#pragma endregion 

#pragma region Weapon Detection

void ABaseCharacter::SetOverlappingWeapon(ABaseWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;

	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABaseCharacter::OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	else if(LastWeapon)
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

		for (const auto Component : GetComponents())
		{
			if(!Component->GetClass()->ImplementsInterface(UCharacterComponent::StaticClass())) continue;
			
			if(const ICharacterComponent* CharComponent = Cast<ICharacterComponent>(Component))
			{
				CharComponent->Execute_SetupInputs(Component, EnhancedInputComponent);
			}
		}
	}
}