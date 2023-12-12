// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BaseProjectile.h"
#include "BulletProjectile.generated.h"

class UDamageProcessingComponent;
/**
 * 
 */
UCLASS()
class HYPERGRAPH_API ABulletProjectile : public ABaseProjectile
{
	GENERATED_BODY()


public:

	ABulletProjectile();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UDamageProcessingComponent* DamageProcessing;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:

	FORCEINLINE UDamageProcessingComponent* GetDamageProcessingComponent() const { return DamageProcessing; }
};
