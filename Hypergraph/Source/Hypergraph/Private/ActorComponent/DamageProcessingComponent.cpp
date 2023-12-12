// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/DamageProcessingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

// Sets default values for this component's properties
UDamageProcessingComponent::UDamageProcessingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageProcessingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDamageProcessingComponent::ProcessDamage(AActor* DamagedActor)
{
	// LEGACY DAMAGE APPLICATION
	//UGameplayStatics::ApplyDamage(DamagedActor, BaseDamage, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());


	if (DamagedActor->Implements<UAbilitySystemInterface>())
	{
		if (IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(DamagedActor))
		{
			if (UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent())
			{
				auto Owner = GetOwner();
				APawn* Instigator = Owner->GetInstigator();

				if (auto InstigatorASI = Cast<IAbilitySystemInterface>(Instigator))
				{
					auto InstigatorASC = InstigatorASI->GetAbilitySystemComponent();
					FGameplayEffectContextHandle ContextHandle = InstigatorASC->MakeEffectContext();

					if (GameplayEffectClass)
					{
						FGameplayEffectSpecHandle SpecHandle = InstigatorASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, ContextHandle);
						FActiveGameplayEffectHandle ActiveHandle = InstigatorASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
				}
			}
		}
	}
}

