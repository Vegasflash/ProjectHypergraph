// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BaseWeapon.h"

#include "Character/BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

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
