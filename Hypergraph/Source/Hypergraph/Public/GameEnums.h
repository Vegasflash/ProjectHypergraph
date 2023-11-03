// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class HYPERGRAPH_API GameEnums
{

	
};

UENUM(BlueprintType)
enum class ECrosshairPosition : uint8
{
	ECP_Center UMETA(DisplayName = "Center"),
	ECP_Top UMETA(DisplayName = "Top"),
	ECP_Bottom UMETA(DisplayName = "Bottom"),
	ECP_Left UMETA(DisplayName = "Left"),
	ECP_Right UMETA(DisplayName = "Right"),
	Count
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Left"),
	ETIP_Right UMETA(DisplayName = "Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),
	
	ETIP_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
};