// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapon/BaseProjectile.h"
#include "GameEnums.h"
#include "ProjectileTarget.generated.h"

UINTERFACE(MinimalAPI)
class UProjectileTarget : public UInterface
{
	GENERATED_BODY()
};

class HYPERGRAPH_API IProjectileTarget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	const ESurfaceType GetSurfaceType();
};
