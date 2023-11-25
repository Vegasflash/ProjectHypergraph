#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "DataAssets/ProjectileDataAsset.h"
#include "BaseProjectile.generated.h"

UCLASS()
class HYPERGRAPH_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

private:
	UFUNCTION(Server, Reliable)
	void Server_BulletHit(ESurfaceType SurfaceType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BulletHit(ESurfaceType SurfaceType);

public:
	ABaseProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Destroyed() override;


	UPROPERTY(EditAnywhere, Category = "Data")
	UProjectileDataAsset* ProjectileData;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComp;

};
