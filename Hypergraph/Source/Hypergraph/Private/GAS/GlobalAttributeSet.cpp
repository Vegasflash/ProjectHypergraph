// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GlobalAttributeSet.h"

UGlobalAttributeSet::UGlobalAttributeSet()
{
	
}

void UGlobalAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, Health, COND_None, REPNOTIFY_Always);
	//
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
}

void UGlobalAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Health, OldHealth);

	OnMaxHealthAttributeRep.Broadcast(OldHealth, Health);
}

void UGlobalAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Health, OldHealth);

	OnHealthAttributeRep.Broadcast(OldHealth, Health);
}

void UGlobalAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Stamina, OldStamina);

	OnMaxStaminaAttributeRep.Broadcast(OldStamina, Stamina);
}

void UGlobalAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Stamina, OldStamina);

	OnStaminaAttributeRep.Broadcast(OldStamina, Stamina);
}

void UGlobalAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	auto Owner = GetOwningActor();
	if (Owner && !Owner->HasAuthority()) return;

	if (Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthAttributeRep.Broadcast(OldValue, NewValue);
	}
	else if (Attribute == GetHealthAttribute())
	{
		OnHealthAttributeRep.Broadcast(OldValue, NewValue);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		OnMaxStaminaAttributeRep.Broadcast(OldValue, NewValue);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		OnStaminaAttributeRep.Broadcast(OldValue, NewValue);
	}
}
