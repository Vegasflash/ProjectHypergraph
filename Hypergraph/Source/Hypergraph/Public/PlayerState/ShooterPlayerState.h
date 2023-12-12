// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "ShooterPlayerState.generated.h"

class AShooterController;
class ABaseCharacter;

/**
 *
 */
UCLASS()
class HYPERGRAPH_API AShooterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	AShooterPlayerState();
	~AShooterPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool ValidateCharacterRef();
	bool ValidateControllerRef();

	TWeakObjectPtr<AShooterController> ShooterController;
	TWeakObjectPtr<ABaseCharacter> BaseCharacter;

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

	UFUNCTION()
	void OnHealthAttributeUpdated(const FGameplayAttributeData& OldHealth, const FGameplayAttributeData& NewHealth);

public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);
	void AddToDeath(float DeathAmount);

	virtual void InitializeAttributes();
	virtual void GiveAbilities();
	void OnShooterCharacterPossesed(ABaseCharacter* ShooterCharacter);
	void OnShooterCharacterStateReplicated(ABaseCharacter* ShooterCharacter);

	class ABaseCharacter* GetCharacter();

	// GAMEPLAY ABILITIES
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;
	///
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE class UGlobalAttributeSet* GetAttributes() { return Attributes; }

	// Inherited via IAbilitySystemInterface
};
