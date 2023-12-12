// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FireWeaponAbilityParams.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct HYPERGRAPH_API FireWeaponAbilityParams
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	FVector ImpactPoint;
};
