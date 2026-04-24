#include "Core/VOIDGameState.h"

void AVOIDGameState::AddScore(int32 Amount)
{
	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore);
}

void AVOIDGameState::SetCurrentWave(int32 WaveIndex)
{
	CurrentWaveIndex = WaveIndex;
	OnWaveChanged.Broadcast(CurrentWaveIndex);
}

void AVOIDGameState::SetRemainingTime(float NewTime)
{
	RemainingTime = NewTime;
	OnTimeChanged.Broadcast(RemainingTime);
}
