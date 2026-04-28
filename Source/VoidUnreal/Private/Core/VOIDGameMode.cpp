#include "Core/VOIDGameMode.h"
#include "Core/VOIDGameState.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Waves/VOIDSpawnVolume.h"
#include "Items/VOIDPickupBase.h"
#include "Characters/VOIDZombieCharacter.h"

AVOIDGameMode::AVOIDGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVOIDGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartWave(1);
}

void AVOIDGameMode::StartWave(int32 WaveIndex)
{
	CurrentWaveIndex = WaveIndex;
	CurrentPhase = EVOIDWavePhase::InProgress;

	UE_LOG(LogTemp, Warning, TEXT("[VOID] StartWave(%d) called. WaveDataTable=%s"),
		WaveIndex, WaveDataTable ? TEXT("OK") : TEXT("NULL"));

	FVOIDWaveData Row;
	if (TryLoadWaveRow(WaveIndex, Row))
	{
		CurrentWaveData = Row;
		UE_LOG(LogTemp, Warning, TEXT("[VOID] Row loaded: ZombieCount=%d ItemSpawnCount=%d TimeLimit=%.1f"),
			Row.ZombieCount, Row.ItemSpawnCount, Row.TimeLimit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[VOID] Row Floor_%d NOT FOUND in DataTable"), WaveIndex);
	}

	SpawnFloor(WaveIndex);

	if (CurrentWaveData.TimeLimit > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			WaveTimerHandle, this, &AVOIDGameMode::OnWaveTimerExpired,
			CurrentWaveData.TimeLimit, false);
	}

	if (AVOIDGameState* GS = GetGameState<AVOIDGameState>())
	{
		GS->SetCurrentWave(WaveIndex);
		GS->SetRemainingTime(CurrentWaveData.TimeLimit);
	}
}

void AVOIDGameMode::EndWave()
{
	CurrentPhase = EVOIDWavePhase::Ending;
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
}

void AVOIDGameMode::AdvanceWave()
{
	if (CurrentWaveIndex >= 3)
	{
		CurrentPhase = EVOIDWavePhase::Completed;
		return;
	}
	EndWave();
	StartWave(CurrentWaveIndex + 1);
}

void AVOIDGameMode::OnWaveTimerExpired()
{
	EndWave();
	AdvanceWave();
}

void AVOIDGameMode::SpawnFloor(int32 FloorIndex)
{
	TArray<AActor*> All;
	UGameplayStatics::GetAllActorsOfClass(this, AVOIDSpawnVolume::StaticClass(), All);

	TArray<AVOIDSpawnVolume*> ZombieVols;
	TArray<AVOIDSpawnVolume*> ItemVols;
	for (AActor* A : All)
	{
		AVOIDSpawnVolume* V = Cast<AVOIDSpawnVolume>(A);
		if (!V || V->FloorIndex != FloorIndex) { continue; }
		if (V->SpawnKind == EVOIDSpawnKind::Zombie || V->SpawnKind == EVOIDSpawnKind::Both) { ZombieVols.Add(V); }
		if (V->SpawnKind == EVOIDSpawnKind::Item   || V->SpawnKind == EVOIDSpawnKind::Both) { ItemVols.Add(V);   }
	}

	UE_LOG(LogTemp, Warning, TEXT("[VOID] SpawnFloor(%d) — ZombieVols=%d ItemVols=%d ZClass=%s PoolSize=%d"),
		FloorIndex, ZombieVols.Num(), ItemVols.Num(),
		CurrentWaveData.ZombieClass ? *CurrentWaveData.ZombieClass->GetName() : TEXT("NULL"),
		CurrentWaveData.PickupPool.Num());

	if (CurrentWaveData.ZombieClass && ZombieVols.Num() > 0)
	{
		TSubclassOf<AActor> ZClass = CurrentWaveData.ZombieClass.Get();
		for (int32 i = 0; i < CurrentWaveData.ZombieCount; ++i)
		{
			ZombieVols[i % ZombieVols.Num()]->SpawnActorOfClass(ZClass);
		}
	}

	if (CurrentWaveData.PickupPool.Num() > 0 && ItemVols.Num() > 0)
	{
		for (int32 i = 0; i < CurrentWaveData.ItemSpawnCount; ++i)
		{
			const int32 Pick = FMath::RandRange(0, CurrentWaveData.PickupPool.Num() - 1);
			TSubclassOf<AActor> PickClass = CurrentWaveData.PickupPool[Pick].Get();
			if (PickClass)
			{
				ItemVols[i % ItemVols.Num()]->SpawnActorOfClass(PickClass);
			}
		}
	}
}

bool AVOIDGameMode::TryLoadWaveRow(int32 WaveIndex, FVOIDWaveData& OutData) const
{
	if (!WaveDataTable) { return false; }

	const FString RowName = FString::Printf(TEXT("Floor_%d"), WaveIndex);
	const FVOIDWaveData* Row = WaveDataTable->FindRow<FVOIDWaveData>(FName(*RowName), TEXT("VOIDGameMode"));
	if (!Row) { return false; }

	OutData = *Row;
	return true;
}
