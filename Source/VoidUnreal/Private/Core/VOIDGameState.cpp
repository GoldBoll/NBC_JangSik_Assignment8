#include "Core/VOIDGameState.h"

// 점수 누적 후 OnScoreChanged Broadcast
void AVOIDGameState::AddScore(int32 Amount)
{
	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore);
}

// 웨이브 인덱스 갱신 후 OnWaveChanged Broadcast
void AVOIDGameState::SetCurrentWave(int32 WaveIndex)
{
	CurrentWaveIndex = WaveIndex;
	OnWaveChanged.Broadcast(CurrentWaveIndex);
}

// 잔여시간 갱신 후 OnTimeChanged Broadcast
void AVOIDGameState::SetRemainingTime(float NewTime)
{
	RemainingTime = NewTime;
	OnTimeChanged.Broadcast(RemainingTime);
}
