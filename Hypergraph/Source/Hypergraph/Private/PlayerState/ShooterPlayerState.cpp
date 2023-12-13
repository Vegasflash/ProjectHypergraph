// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"
#include "Controller/ShooterController.h"
#include "Character/BaseCharacter.h"
#include "GAS/GlobalAttributeSet.h"

AShooterPlayerState::AShooterPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UGlobalAttributeSet>(TEXT("Attributes"));
	Attributes->OnHealthAttributeRep.AddDynamic(this, &AShooterPlayerState::OnHealthAttributeUpdated);
}

AShooterPlayerState::~AShooterPlayerState()
{

}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, DeathCount);
}

bool AShooterPlayerState::ValidateCharacterRef()
{
	auto Pawn = GetPawn();

	if (Pawn == nullptr) return false;
	if (!BaseCharacter.IsValid()) { BaseCharacter = MakeWeakObjectPtr<ABaseCharacter>(Cast<ABaseCharacter>(Pawn)); }

	return BaseCharacter.IsValid();
}

bool AShooterPlayerState::ValidateControllerRef()
{
	auto Pawn = GetPawn();

	if (Pawn == nullptr) return false;
	if (!ShooterController.IsValid()) { ShooterController = MakeWeakObjectPtr<AShooterController>(Cast<AShooterController>(Pawn->GetController())); }

	return ShooterController.IsValid();
}

void AShooterPlayerState::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

	}
}

void AShooterPlayerState::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1));
		}
	}
}

void AShooterPlayerState::OnHealthAttributeUpdated(const FGameplayAttributeData& OldHealth, const FGameplayAttributeData& NewHealth)
{
	if (ValidateCharacterRef())
	{
		auto role = BaseCharacter->GetLocalRole();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(role));

		if (NewHealth.GetCurrentValue() < 0)
		{
			BaseCharacter->Elim();
		}

		BaseCharacter->RefreshHUD_Server();
	}
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if (ValidateControllerRef())
	{
		ShooterController->SetHUDKills(GetScore());
	}
}

void AShooterPlayerState::OnRep_DeathCount(int PrevDeathCount)
{
	if (ValidateControllerRef())
	{
		ShooterController->SetHUDDeaths(DeathCount);
	}
}

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	if (ValidateControllerRef())
	{
		ShooterController->SetHUDKills(GetScore());
	}
}

void AShooterPlayerState::AddToDeath(float DeathAmount)
{
	DeathCount += DeathAmount;

	if (ValidateControllerRef())
	{
		ShooterController->SetHUDDeaths(DeathCount);
	}
}

// This is called from the Controller, and therefore from the server, not the client.
void AShooterPlayerState::OnShooterCharacterPossesed(ABaseCharacter* ShooterCharacter)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, ShooterCharacter);

		InitializeAttributes();
		GiveAbilities();
	}
}

// This is called from the Character when it finally gets it's Player State replicated on client.
void AShooterPlayerState::OnShooterCharacterStateReplicated(ABaseCharacter* ShooterCharacter)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, ShooterCharacter);

		InitializeAttributes();
	}
}

ABaseCharacter* AShooterPlayerState::GetCharacter()
{
	if (ValidateCharacterRef())
	{
		return BaseCharacter.Get();
	}
	return nullptr;
}

UAbilitySystemComponent* AShooterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


