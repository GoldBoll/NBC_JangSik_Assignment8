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

	const float Before = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);

	UE_LOG(LogTemp, Warning, TEXT("[Health] %s ApplyDamage(%.1f) — HP %.1f → %.1f / %.1f%s"),
		*GetNameSafe(GetOwner()), Amount, Before, CurrentHealth, MaxHealth,
		IsDead() ? TEXT(" [DEAD]") : TEXT(""));

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
