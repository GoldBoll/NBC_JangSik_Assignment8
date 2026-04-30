#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VOIDGameState.generated.h"

// 점수/웨이브/시간 변동 시 HUD가 구독해서 갱신하는 멀티캐스트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnWaveChanged, int32, NewWaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnTimeChanged, float, RemainingTime);

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// 점수 누적 (좀비 처치/아이템 획득 시) + Broadcast
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScore(int32 Amount);

	// 현재 웨이브 번호 갱신 + Broadcast (GameMode가 호출)
	UFUNCTION(BlueprintCallable, Category="Wave")
	void SetCurrentWave(int32 WaveIndex);

	// 잔여 시간 갱신 + Broadcast (1초 Tick으로 호출됨)
	UFUNCTION(BlueprintCallable, Category="Wave")
	void SetRemainingTime(float NewTime);

	// 현재 점수 조회
	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetCurrentScore() const { return CurrentScore; }

	// 현재 웨이브 번호 조회
	UFUNCTION(BlueprintPure, Category="Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	// 잔여 시간 조회 (HUD 바인딩용)
	UFUNCTION(BlueprintPure, Category="Wave")
	float GetRemainingTime() const { return RemainingTime; }

	// 점수 변경 이벤트 — HUD ScoreText 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnScoreChanged OnScoreChanged;

	// 웨이브 변경 이벤트 — HUD WaveText 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnWaveChanged OnWaveChanged;

	// 시간 변경 이벤트 — HUD TimeText 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnTimeChanged OnTimeChanged;

protected:
	// 누적 점수 (좀비 처치 + 아이템 가치)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 CurrentScore = 0;

	// 현재 웨이브 인덱스 (1~3)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 CurrentWaveIndex = 0;

	// 탈출 잔여 시간(초)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	float RemainingTime = 0.0f;
};
