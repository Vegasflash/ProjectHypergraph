// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 *
 */

UCLASS()
class HYPERGRAPH_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()


public:
	const static float DEFAULT_ZOOMED_FOV;
	const static float DEFAULT_ZOOM_INTERP_SPEED;


	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomedFoV = DEFAULT_ZOOMED_FOV;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomInterpSpeed = DEFAULT_ZOOM_INTERP_SPEED;

public:
	inline const float GetZoomedFoV() const { return ZoomedFoV; }
	inline const float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

};

const float UWeaponDataAsset::DEFAULT_ZOOMED_FOV = 30.f;
const float UWeaponDataAsset::DEFAULT_ZOOM_INTERP_SPEED = 20.f;