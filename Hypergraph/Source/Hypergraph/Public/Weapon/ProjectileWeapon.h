// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BaseWeapon.h"
#include "BaseProjectile.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere, Category="Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	virtual void Fire(const FVector& HitTarget) override;
	
public:
	FORCEINLINE TSubclassOf<ABaseProjectile> GetProjectileClass() const { return ProjectileClass; }
};
