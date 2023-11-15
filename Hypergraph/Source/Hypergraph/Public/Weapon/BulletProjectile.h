// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BaseProjectile.h"
#include "ActorComponent/DamageProcessingComponent.h"
#include "BulletProjectile.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API ABulletProjectile : public ABaseProjectile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UDamageProcessingComponent* DamageProcessing;

public:

	ABulletProjectile();


protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:

	FORCEINLINE UDamageProcessingComponent* GetDamageProcessingComponent() const { return DamageProcessing; }
};
