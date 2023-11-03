// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Cartridge.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACartridge::ACartridge()
	: ShellEjectionImpulse(10)
	, MaxLifeTime(3)
	, bCanPlayHitSound(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("StaticMesh")));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetEnableGravity(true);

	CurrentLifeTime = MaxLifeTime;
}

// Called when the game starts or when spawned
void ACartridge::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ACartridge::OnHit);

	FRotator rotation = FRotator(0, 0, FMath::RandRange(-45.f, 45.f));
	auto EjectDir = rotation.RotateVector(-GetActorRightVector());

	StaticMeshComponent->AddImpulse(EjectDir * ShellEjectionImpulse);
}

void ACartridge::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound && bCanPlayHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		bCanPlayHitSound = false;
	}
}

// Called every frame
void ACartridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentLifeTime -= DeltaTime;
	if (CurrentLifeTime < 0)
	{
		Destroy();
	}
}

