#include "Characters/VOIDZombieCharacter.h"
#include "Components/VOIDHealthComponent.h"
#include "Core/VOIDGameState.h"
#include "Kismet/GameplayStatics.h"

AVOIDZombieCharacter::AVOIDZombieCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVOIDZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (auto* HP = FindComponentByClass<UVOIDHealthComponent>())
	{
		HP->OnDeath.AddUniqueDynamic(this, &AVOIDZombieCharacter::HandleZombieDeath);
	}
}

void AVOIDZombieCharacter::HandleZombieDeath()
{
	if (auto* GS = UGameplayStatics::GetGameState(this))
	{
		if (auto* VGS = Cast<AVOIDGameState>(GS))
		{
			VGS->AddScore(ScoreReward);
			UE_LOG(LogTemp, Display, TEXT("[Zombie] %s killed → +%d score (total=%d)"),
				*GetName(), ScoreReward, VGS->GetCurrentScore());
		}
	}
}

void AVOIDZombieCharacter::AttackPlayer(AActor* Target)
{
	// 쿨다운 체크
	if (!bCanAttack || !Target) return;

	// 거리 체크 (AttackRange = 100)
	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > AttackRange) return;

	// 데미지 적용
	if (auto* TargetCharacter = Cast<AVOIDBaseCharacter>(Target))
	{
		TargetCharacter->ApplyDamage(AttackDamage);
		UE_LOG(LogTemp, Warning, TEXT("Zombie hit %s for %.1f"),
		       *Target->GetName(), AttackDamage);
	}

	// 쿨다운 시작
	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownHandle,
		[this]() { bCanAttack = true; },
		AttackCooldown,
		false
	);
}
