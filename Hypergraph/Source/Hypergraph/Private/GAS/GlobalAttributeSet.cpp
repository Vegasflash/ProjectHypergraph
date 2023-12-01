// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GlobalAttributeSet.h"

UGlobalAttributeSet::UGlobalAttributeSet()
{
	
}

void UGlobalAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGlobalAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
}

void UGlobalAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Health, OldHealth);
}

void UGlobalAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGlobalAttributeSet, Stamina, OldStamina);
}
