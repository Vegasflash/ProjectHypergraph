#include "Weapon/BaseProjectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Character/BaseCharacter.h"
#include "Hypergraph.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// start by turning off collisions and then specifically allowing Visibility and world static channels.
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectilveMovementComponent"));
	ProjectileMovementComp->bRotationFollowsVelocity = true;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComp = UGameplayStatics::SpawnEmitterAttached(Tracer, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	}

}


void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto HitSurface = OtherActor->Implements<UProjectileTarget>() ?
		IProjectileTarget::Execute_GetSurfaceType(OtherActor) :
		ESurfaceType::Default;

	Server_BulletHit(HitSurface);

	Destroy();
}

void ABaseProjectile::Server_BulletHit_Implementation(ESurfaceType SurfaceType)
{
	Multicast_BulletHit(SurfaceType);
}

void ABaseProjectile::Multicast_BulletHit_Implementation(ESurfaceType SurfaceType)
{
	if (ProjectileData != nullptr)
	{
		FProjectileImpactData ImpactData = ProjectileData->GetImpactData()[SurfaceType];
		if (ImpactData.ImpactFX)
		{

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactData.ImpactFX, GetActorTransform());
		}

		if (ImpactData.ImpactSFX)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactData.ImpactSFX, GetActorLocation());
		}
	}
}

void ABaseProjectile::Destroyed()
{
	Super::Destroyed();
}

void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

