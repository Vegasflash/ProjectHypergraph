// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameEnums.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, Category="Weapoon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	void SetOverlapDetection(bool bActive);

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
