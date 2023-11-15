// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "GameEnums.h"
#include "ProjectileDataAsset.generated.h"


USTRUCT()
struct FProjectileImpactData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "ImpactData")
	UParticleSystem* ImpactFX;

	UPROPERTY(EditAnywhere, Category = "ImpactData")
	USoundCue* ImpactSFX;
};

UCLASS()
class HYPERGRAPH_API UProjectileDataAsset : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "ImpactData")
	TMap<ESurfaceType, FProjectileImpactData> ImpactData;

public:
	FORCEINLINE const TMap<ESurfaceType, FProjectileImpactData>& GetImpactData() const { return ImpactData; }
};
