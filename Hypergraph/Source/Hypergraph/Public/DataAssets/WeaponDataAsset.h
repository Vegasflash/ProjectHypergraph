// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameEnums.h"
#include "ProjectileDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WeaponDataAsset.generated.h"

/**
 *
 */

UCLASS()
class HYPERGRAPH_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()


public:
	const static EFiringMode DEFAULT_FIRING_MODE;
	const static float DEFAULT_TIME_BETWEEEN_SHOTS;
	const static float DEFAULT_ZOOM_INTERP_SPEED;
	const static float DEFAULT_ZOOMED_FOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	EFiringMode DefaultFiringMode;
	
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (Bitmask, BitmaskEnum = EFiringMode))
	int32 FiringModes;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float TimeBetweenShotsInterval = DEFAULT_TIME_BETWEEEN_SHOTS;


	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomInterpSpeed = DEFAULT_ZOOM_INTERP_SPEED;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomedFoV = DEFAULT_ZOOMED_FOV;
	
	UPROPERTY(EditAnywhere, Category = "Zoom")
	UProjectileDataAsset* ProjectileDataAsset;


public:
	inline const EFiringMode GetDefaultFiringMode() const { return DefaultFiringMode; }
	inline const int32 GetFiringModes() const { return FiringModes; }
	inline const float GetTimeBetweenShotsInterval() const { return TimeBetweenShotsInterval; }
	inline const float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	inline const float GetZoomedFoV() const { return ZoomedFoV; }

};

const EFiringMode UWeaponDataAsset::DEFAULT_FIRING_MODE = EFiringMode::EFM_Single;
const float UWeaponDataAsset::DEFAULT_TIME_BETWEEEN_SHOTS = 0.1f;
const float UWeaponDataAsset::DEFAULT_ZOOM_INTERP_SPEED = 20.f;
const float UWeaponDataAsset::DEFAULT_ZOOMED_FOV = 30.f;