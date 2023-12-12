#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "DataAssets/ProjectileDataAsset.h"
#include "BaseProjectile.generated.h"

class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBulletHitAbilityUser, UAbilitySystemComponent*, AbilityUser, EDirection, LastHitDir, ESurfaceType, SurfaceType);

UCLASS()
class HYPERGRAPH_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

private:
	UFUNCTION(Server, Reliable)
	void Server_BulletHit(AActor* OtherActor);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BulletHit(ESurfaceType SurfaceType);

	TWeakPtr<AActor> ProjectileInstigator;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComp;
public:
	ABaseProjectile();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnBulletHitAbilityUser OnAbilityUserHit;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Destroyed() override;


	UPROPERTY(EditAnywhere, Category = "Data")
	UProjectileDataAsset* ProjectileData;

public:
	void SetProjectileInstigator(AActor* Instigator);
	TWeakPtr<AActor> GetProjectileInstigator() { return ProjectileInstigator; }
};
