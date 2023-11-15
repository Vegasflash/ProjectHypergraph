// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BulletProjectile.h"
#include "Interfaces/Damageable.h"



ABulletProjectile::ABulletProjectile()
{
	DamageProcessing = CreateDefaultSubobject<UDamageProcessingComponent>(DEFAULT_COMPONENT_NAME);
}

void ABulletProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->Implements<UDamageable>())
	{
		DamageProcessing->ProcessDamage(OtherActor);
	}

	// Destroy is called in the base OnHit().
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
