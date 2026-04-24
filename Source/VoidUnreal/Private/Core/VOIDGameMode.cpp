#include "Core/VOIDGameMode.h"

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
	StartWave(CurrentWaveIndex + 1);
}

void AVOIDGameMode::OnWaveTimerExpired()
{
	EndWave();
	AdvanceWave();
}
