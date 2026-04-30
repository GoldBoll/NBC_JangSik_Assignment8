#include "Components/VOIDHealthComponent.h"

UVOIDHealthComponent::UVOIDHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// CurrentHealth = MaxHealth 초기화
void UVOIDHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

// 데미지 적용 → OnHealthChanged Broadcast, HP 0이 되면 OnDeath
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

// HP 회복, MaxHealth 상한 클램프 후 OnHealthChanged Broadcast
void UVOIDHealthComponent::Heal(float Amount)
{
	if (IsDead()) { return; }

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
}
