#include "Components/VOIDHealthComponent.h"

UVOIDHealthComponent::UVOIDHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVOIDHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void UVOIDHealthComponent::ApplyDamage(float Amount)
{
	if (IsDead()) { return; }

	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

void UVOIDHealthComponent::Heal(float Amount)
{
	if (IsDead()) { return; }

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
}
