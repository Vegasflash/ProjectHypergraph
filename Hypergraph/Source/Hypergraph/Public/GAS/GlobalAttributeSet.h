// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "GlobalAttributeSet.generated.h"

// Uses macros for AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) \
 GAMEPLAYATRIBUTE_BASE_VALUE_GETTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeRep, const FGameplayAttributeData&, OldMaxHealth, const FGameplayAttributeData&, NewMaxHealth);
/**
 * 
 */
UCLASS()
class HYPERGRAPH_API UGlobalAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public :
	UGlobalAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// MAX HEALTH
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGlobalAttributeSet, MaxHealth);
	UFUNCTION() virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UPROPERTY() FOnAttributeRep OnMaxHealthAttributeRep;
	// HEALTH
	UPROPERTY(BlueprintReadOnly, Category ="Attributes", ReplicatedUsing = OnRep_Health) FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGlobalAttributeSet, Health);
	UFUNCTION() virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UPROPERTY() FOnAttributeRep OnHealthAttributeRep;
	//


	// MAX STAMINA
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UGlobalAttributeSet, MaxStamina);
	UFUNCTION() virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	UPROPERTY() FOnAttributeRep OnMaxStaminaAttributeRep;
	// STAMINA
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UGlobalAttributeSet, Stamina);
	UFUNCTION() virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	UPROPERTY() FOnAttributeRep OnStaminaAttributeRep;
	//

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
