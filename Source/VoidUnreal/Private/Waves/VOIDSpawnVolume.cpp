#include "Waves/VOIDSpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

AVOIDSpawnVolume::AVOIDSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Bounds->SetBoxExtent(FVector(500.0f, 500.0f, 100.0f));
	Bounds->SetCollisionProfileName(TEXT("NoCollision"));
	RootComponent = Bounds;
}

FVector AVOIDSpawnVolume::GetRandomPointInVolume() const
{
	if (!Bounds) { return GetActorLocation(); }

	const FVector Extent = Bounds->GetScaledBoxExtent();
	const FVector Origin = GetActorLocation();
	const FVector Candidate = Origin + FVector(
		FMath::FRandRange(-Extent.X, Extent.X),
		FMath::FRandRange(-Extent.Y, Extent.Y),
		Extent.Z);

	// 후보 지점에서 바닥으로 라인 트레이스 → 픽업/좀비가 항상 바닥에 안착
	if (UWorld* World = GetWorld())
	{
		FHitResult Hit;
		const FVector TraceEnd = Candidate - FVector(0.0f, 0.0f, Extent.Z * 2.0f + 500.0f);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		if (World->LineTraceSingleByChannel(Hit, Candidate, TraceEnd, ECC_Visibility, Params))
		{
			return Hit.ImpactPoint;
		}
	}
	return Candidate;
}

AActor* AVOIDSpawnVolume::SpawnActorOfClass(TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass) { return nullptr; }

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<AActor>(ActorClass, GetRandomPointInVolume(), FRotator::ZeroRotator, Params);
}
