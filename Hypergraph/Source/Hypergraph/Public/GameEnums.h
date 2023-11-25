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

	Count UMETA(Hidden)
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
	
	ETIP_MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	ED_Forward UMETA(DisplayName = "Forward"),
	ED_Right UMETA(DisplayName = "Right"),
	ED_Back UMETA(DisplayName = "Back"),
	ED_Left UMETA(DisplayName = "Left")
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFiringMode : uint8
{
	NONE = 0 UMETA(Hidden),
	EFM_Single = 1 << 0 UMETA(DisplayName = "Single"),
	EFM_Burst = 1 << 1 UMETA(DisplayName = "Burst"),
	EFM_FullAuto = 1 << 2 UMETA(DisplayName = "FullAuto"),
};
ENUM_CLASS_FLAGS(EFiringMode);
// Suggested path name for the enum type: This is required to remove the warning.
#define PATH_EFiringMode "/Script/Hypergraph.EFiringMode"

UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	Default UMETA(DisplayName = "Default"),
	Wood UMETA(DisplayName = "Wood"),
	Stone UMETA(DisplayName = "Stone"),
	Metal UMETA(DisplayName = "Metal"),
	Water UMETA(DisplayName = "Water"),
	Flesh UMETA(DisplayName = "Flesh"),
};