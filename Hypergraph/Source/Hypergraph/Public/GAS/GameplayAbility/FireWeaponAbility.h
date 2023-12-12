// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FireWeaponAbility.generated.h"

/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UFireWeaponAbility : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> BulletHitEffect;

	TWeakObjectPtr<AShooterPlayerState> AbilityOwner;
public:
	UFireWeaponAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// Function to spawn a bullet
	void SpawnBullet(class ABaseCharacter* Character);

	UFUNCTION()
	void OnBulletHitAbilityUser(UAbilitySystemComponent* TargetASC, EDirection LastHitDirection, ESurfaceType SurfaceType);
};
