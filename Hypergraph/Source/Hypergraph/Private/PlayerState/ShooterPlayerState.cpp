// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"


AShooterPlayerState::AShooterPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UGlobalAttributeSet>(TEXT("Attributes"));
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, DeathCount);
}

void AShooterPlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	Super::RegisterPlayerWithSession(bWasFromInvite);

	if (auto Controller = Cast<AShooterController>(GetPlayerController()))
	{
		Controller->InitEarlyPlayerState(this);
	}
}

bool AShooterPlayerState::ValidatePlayerRefs()
{
	auto Pawn = GetPawn();
	if (Character == nullptr) { Character = Cast<ABaseCharacter>(Pawn); }
	if (PlayerController == nullptr) { PlayerController = Cast<AShooterController>(Pawn->GetController()); }

	return Character && PlayerController;
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

void AShooterPlayerState::OnRep_DeathCount(int PrevDeathCount)
{
	if (ValidatePlayerRefs())
	{
		PlayerController->SetHUDDeaths(DeathCount);
	}
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if (ValidatePlayerRefs())
	{
		PlayerController->SetHUDKills(GetScore());
	}
}

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	if (ValidatePlayerRefs())
	{
		PlayerController->SetHUDKills(GetScore());
	}
}

void AShooterPlayerState::AddToDeath(float DeathAmount)
{
	DeathCount += DeathAmount;

	if (ValidatePlayerRefs())
	{
		PlayerController->SetHUDDeaths(DeathCount);
	}
}

void AShooterPlayerState::OnShooterCharacterPossesed(ABaseCharacter* ShooterCharacter)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, ShooterCharacter);

		InitializeAttributes();
		GiveAbilities();
	}
}

void AShooterPlayerState::OnShooterCharacterStateReplicated(ABaseCharacter* ShooterCharacter)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, ShooterCharacter);

		InitializeAttributes();
	}
}


