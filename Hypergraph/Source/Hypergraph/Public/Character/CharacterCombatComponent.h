// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "ICharacterComponent.h"
#include "CharacterCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HYPERGRAPH_API UCharacterCombatComponent : public UActorComponent, public ICharacterComponent
{
	GENERATED_BODY()

public:
	UCharacterCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category= Input)
	void SetupInputs(UEnhancedInputComponent* PlayerInputComponent);
	virtual void SetupInputs_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;
};
