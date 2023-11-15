// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameEnums.h"
#include "GameFramework/Actor.h"
#include "Cartridge.h"
#include "DataAssets/WeaponDataAsset.h"
#include "BaseWeapon.generated.h"


UCLASS()
class HYPERGRAPH_API ABaseWeapon : public AActor
{
	GENERATED_BODY()


public:
	ABaseWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Fire(const FVector& HitTarget);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPawnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                        const FHitResult& SweepResult);

	UFUNCTION()
	void OnPawnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                      int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category= WeaponProperties)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category= WeaponProperties)
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category= UI)
	class UWidgetComponent* PickupPromptWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACartridge> Cartridge;

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, Category="Weapoon Properties")
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TMap<ECrosshairPosition, UTexture2D*> CrosshairTextures;

	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	UWeaponDataAsset* WeaponData;

	UFUNCTION()
	void OnRep_WeaponState();

	void SetOverlapDetection(bool bActive);

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category="Crosshair", meta = (ClampMin = "1.0", ClampMax = "5.0", UIMin = "0", UIMax = "5.0"))
	float RecoilIntensity;

	void RotatePickupWidgetTowardsPlayerCam();

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	//Crosshair
	FORCEINLINE float GetRecoilIntensity() const { return RecoilIntensity; }
	FORCEINLINE const TMap<ECrosshairPosition, UTexture2D*> GetCrosshairTextures() const { return CrosshairTextures; }

	// Weapon Data
	const UWeaponDataAsset* GetWeaponData() const { return WeaponData; }
};
