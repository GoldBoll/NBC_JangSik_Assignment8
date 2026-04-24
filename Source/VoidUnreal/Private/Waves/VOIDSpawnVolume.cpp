#include "Waves/VOIDSpawnVolume.h"
#include "Components/BoxComponent.h"

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
	return Origin + FVector(
		FMath::FRandRange(-Extent.X, Extent.X),
		FMath::FRandRange(-Extent.Y, Extent.Y),
		0.0f);
}

AActor* AVOIDSpawnVolume::SpawnActorOfClass(TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass) { return nullptr; }

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<AActor>(ActorClass, GetRandomPointInVolume(), FRotator::ZeroRotator, Params);
}
