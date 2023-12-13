// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/FireWeaponAbility.h"
#include "Character/BaseCharacter.h"
#include "Weapon/ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PlayerState/ShooterPlayerState.h"
#include "Kismet//GameplayStatics.h"

UFireWeaponAbility::UFireWeaponAbility()
{
}

void UFireWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AbilityOwner = MakeWeakObjectPtr<AShooterPlayerState>(Cast<AShooterPlayerState>(ActorInfo->OwnerActor));
	//if (!AbilityOwner->HasAuthority())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Client Fire Weapon Ability Activated!"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Server Fire Weapon Ability Activated!"));
	//}

	if (AbilityOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Tried activating an ability when the owner is not the player state..."));
		return;
	}

	if (ABaseCharacter* Shooter = AbilityOwner->GetCharacter())
	{
		SpawnBullet(Shooter);
	}
}

void UFireWeaponAbility::SpawnBullet(ABaseCharacter* Character)
{
	UCharacterCombatComponent* Combat = Character->GetCharacterCombat();
	if (Combat == nullptr) return;

	ABaseWeapon* Weapon = Character->GetCharacterCombat()->GetEquippedWeapon();
	APawn* InstigatorPawn = Cast<APawn>(Character);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Weapon;
	SpawnParams.Instigator = InstigatorPawn;

	auto WeaponMesh = Weapon->GetWeaponMesh();

	if (auto MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash")))
	{
		FVector MuzzleLocation;
		FRotator MuzzleRotation;
		FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);

		FVector CrosshairHitPosition = Combat->GetHitPosition();

		// Calculate rotation to hit location
		FTransform TargetTransform = FTransform::Identity;
		TargetTransform.SetLocation(CrosshairHitPosition);
		FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetTransform.GetLocation() - MuzzleTransform.GetLocation()).Rotator();

		if (UWorld* World = GetWorld())
		{
			if (AProjectileWeapon* ProjWeapon = Cast<AProjectileWeapon>(Weapon))
			{
				auto Projectile = World->SpawnActor<ABaseProjectile>(ProjWeapon->GetProjectileClass(), MuzzleTransform.GetLocation(), TargetRotation, SpawnParams);
				Projectile->OnAbilityUserHit.AddDynamic(this, &ThisClass::OnBulletHitAbilityUser);
			}

			Weapon->Fire(CrosshairHitPosition);
			Combat->FireWeapon_Server();
		}
	}
}

void UFireWeaponAbility::OnBulletHitAbilityUser(UAbilitySystemComponent* TargetASC, EDirection LastHitDirection, ESurfaceType SurfaceType)
{
	if (!AbilityOwner.IsValid()) return;

	if (AbilityOwner->HasAuthority())
	{

		FGameplayEffectContextHandle Handle;
		auto SpecHandle = AbilityOwner->GetAbilitySystemComponent()->MakeOutgoingSpec(BulletHitEffect, 1, Handle);

		SpecHandle.Data->AddDynamicAssetTag(FGameplayTag::RequestGameplayTag(GameEnums::GetSurfaceTagName(SurfaceType)));

		if (SpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// Make Sure this is called AFTER applying Game Effect.
	if (auto HitChararacter = Cast<ABaseCharacter>(TargetASC->GetAvatarActor()))
	{
		HitChararacter->Server_HitDetected(AbilityOwner->GetPlayerController(), LastHitDirection);
	}
}

