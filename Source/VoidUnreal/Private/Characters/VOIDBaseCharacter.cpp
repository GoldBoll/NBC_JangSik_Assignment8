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

	// BP 측에서 HealthComponent 가 누락된 경우 런타임 fallback
	if (!HealthComponent)
	{
		HealthComponent = FindComponentByClass<UVOIDHealthComponent>();
		if (!HealthComponent)
		{
			HealthComponent = NewObject<UVOIDHealthComponent>(this, TEXT("HealthComponent_Runtime"));
			if (HealthComponent)
			{
				HealthComponent->RegisterComponent();
				UE_LOG(LogTemp, Warning, TEXT("[Base] %s HealthComponent runtime-created (BP missing)"), *GetName());
			}
		}
	}
}

void AVOIDBaseCharacter::ApplyDamage(float DamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("[Base] %s::ApplyDamage(%.1f)  HealthComp=%s"),
		*GetName(), DamageAmount,
		HealthComponent ? *HealthComponent->GetName() : TEXT("NULL"));

	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(DamageAmount);
	}
}

bool AVOIDBaseCharacter::IsDead() const
{
	return HealthComponent && HealthComponent->IsDead();
}
