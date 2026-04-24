#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VOIDGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnWaveChanged, int32, NewWaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnTimeChanged, float, RemainingTime);

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Wave")
	void SetCurrentWave(int32 WaveIndex);

	UFUNCTION(BlueprintCallable, Category="Wave")
	void SetRemainingTime(float NewTime);

	UFUNCTION(BlueprintPure, Category="Score")
	int32 GetCurrentScore() const { return CurrentScore; }

	UFUNCTION(BlueprintPure, Category="Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	UFUNCTION(BlueprintPure, Category="Wave")
	float GetRemainingTime() const { return RemainingTime; }

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnWaveChanged OnWaveChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnTimeChanged OnTimeChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Score")
	int32 CurrentScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 CurrentWaveIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	float RemainingTime = 0.0f;
};
