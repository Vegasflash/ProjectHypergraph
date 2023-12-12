// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BaseWeapon.h"

#include "Character/BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABaseWeapon::ABaseWeapon() :
	RecoilIntensity(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	//SetReplicates(true);
	bReplicates = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon Mesh");
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>("Area Sphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupPromptWidget = CreateDefaultSubobject<UWidgetComponent>("Pickup Prompt Widget");
	PickupPromptWidget->SetupAttachment(RootComponent);
}



void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetOverlapDetection(true);
	ShowPickupWidget(false);
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotatePickupWidgetTowardsPlayerCam();
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, WeaponState);
}

void ABaseWeapon::Fire(const FVector& HitTarget)
{
	if(FireAnimation == nullptr) return;
	
	if (WeaponMesh == nullptr) return;

	WeaponMesh->PlayAnimation(FireAnimation, false);

	if (Cartridge == nullptr) return;

	auto Transform = WeaponMesh->GetSocketTransform(FName("AmmoEject"));
	if (auto World = GetWorld())
	{
		World->SpawnActor<ACartridge>(Cartridge, Transform);
	}

}

void ABaseWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);

	FDetachmentTransformRules DettachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DettachRules);
	SetOwner(nullptr);
}

void ABaseWeapon::GetWorldTransform_Implementation(FTransform& Transform)
{
	Transform = GetTransform();
}

void ABaseWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	SetOverlapDetection(false);
	ShowPickupWidget(false);
}

void ABaseWeapon::OnPawnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter && PickupPromptWidget)
	{
		BaseCharacter->SetOverlappingWeapon(this);
	}
}

void ABaseWeapon::OnPawnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                   AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter && PickupPromptWidget)
	{
		BaseCharacter->SetOverlappingWeapon(nullptr);
	}
}

void ABaseWeapon::RotatePickupWidgetTowardsPlayerCam()
{
	if (PickupPromptWidget != nullptr && PickupPromptWidget->IsVisible())
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

			// Calculate the rotation needed to face the camera
			FRotator WidgetRotation = FRotationMatrix::MakeFromX(CameraLocation - GetActorLocation()).Rotator();

			// Set the widget component's rotation
			PickupPromptWidget->SetWorldRotation(WidgetRotation);
		}
	}
}

void ABaseWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (State)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}


void ABaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
		case EWeaponState::EWS_Initial: break;
		case EWeaponState::EWS_Equipped:
			ShowPickupWidget(false);
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetEnableGravity(false);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EWeaponState::EWS_Dropped:
			WeaponMesh->SetSimulatePhysics(true);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
	
		case EWeaponState::EWS_MAX: break;
		default: ;
	}
}

void ABaseWeapon::SetOverlapDetection(bool bActive)
{
	// We only want to activate overlaps on the Authoritative Server. -rbbeaulieu
	if (HasAuthority())
	{
		if (AreaSphere)
		{
			if (bActive)
			{
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
				AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnPawnOverlapBegin);
				AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnPawnOverlapEnd);
			}
			else
			{
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
				AreaSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ABaseWeapon::OnPawnOverlapBegin);
				AreaSphere->OnComponentEndOverlap.RemoveDynamic(this, &ABaseWeapon::OnPawnOverlapEnd);
			}
		}
	}
}

void ABaseWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupPromptWidget)
	{
		PickupPromptWidget->SetVisibility(bShowWidget);
	}
}
