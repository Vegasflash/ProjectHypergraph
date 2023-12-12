// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageProcessingComponent.generated.h"

#define DEFAULT_COMPONENT_NAME FName("Damage Processing Component")

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYPERGRAPH_API UDamageProcessingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void ProcessDamage(AActor* DamagedActor);

private:
	// Sets default values for this component's properties
	UDamageProcessingComponent();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float BaseDamage = 5.0f;

	UPROPERTY(EditAnywhere)
	class TSubclassOf<class UGameplayEffect> GameplayEffectClass;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

};
