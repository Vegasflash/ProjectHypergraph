// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Controller/ShooterController.h"
#include "Character/BaseCharacter.h"

#include "ShooterPlayerState.generated.h"

/**
 *
 */
UCLASS()
class HYPERGRAPH_API AShooterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	AShooterPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void RegisterPlayerWithSession(bool bWasFromInvite) override;
	bool ValidatePlayerRefs();

	AShooterController* PlayerController;
	ABaseCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_DeathCount)
	int DeathCount = 0;

	UFUNCTION()
	void OnRep_DeathCount(int PrevDeathCount);

	// GAMEPLAY ABILITIES
	UPROPERTY()
	TObjectPtr<class UGlobalAttributeSet> Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	///

public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);
	void AddToDeath(float DeathAmount);

	virtual void InitializeAttributes();
	virtual void GiveAbilities();
	void OnShooterCharacterPossesed(ABaseCharacter* ShooterCharacter);
	void OnShooterCharacterStateReplicated(ABaseCharacter* ShooterCharacter);

	// GAMEPLAY ABILITIES
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
	///

	FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
};
