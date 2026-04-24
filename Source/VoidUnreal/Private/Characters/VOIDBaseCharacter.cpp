#include "Characters/VOIDBaseCharacter.h"
#include "Components/VOIDHealthComponent.h"

AVOIDBaseCharacter::AVOIDBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	HealthComponent = CreateDefaultSubobject<UVOIDHealthComponent>(TEXT("HealthComponent"));
}

void AVOIDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AVOIDBaseCharacter::ApplyDamage(float DamageAmount)
{
	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(DamageAmount);
	}
}

bool AVOIDBaseCharacter::IsDead() const
{
	return HealthComponent && HealthComponent->IsDead();
}
