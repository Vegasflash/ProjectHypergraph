// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	FActorSpawnParameters SpawnParams;
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = InstigatorPawn;


	if (auto MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash")))
	{
		auto MuzzleSocketT = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		// From muzzle flash soocket to hit location in params.
		FVector ToTarget = HitTarget - MuzzleSocketT.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (UWorld* World = GetWorld())
		{
			auto Projectile = World->SpawnActor<ABaseProjectile>(ProjectileClass, MuzzleSocketT.GetLocation(), TargetRotation,
			                                    SpawnParams);
		}
	}
}
