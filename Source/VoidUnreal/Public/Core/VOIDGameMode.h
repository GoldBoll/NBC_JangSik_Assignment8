#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Waves/VOIDWaveData.h"
#include "VOIDGameMode.generated.h"

class UDataTable;

UENUM(BlueprintType)
enum class EVOIDWavePhase : uint8
{
	Preparing      UMETA(DisplayName="Preparing"),
	InProgress     UMETA(DisplayName="InProgress"),
	Ending         UMETA(DisplayName="Ending"),
	Completed      UMETA(DisplayName="Completed")
};

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVOIDGameMode();

	UFUNCTION(BlueprintCallable, Category="Wave")
	void StartWave(int32 WaveIndex);

	UFUNCTION(BlueprintCallable, Category="Wave")
	void EndWave();

	UFUNCTION(BlueprintCallable, Category="Wave")
	void AdvanceWave();

	UFUNCTION(BlueprintPure, Category="Wave")
	EVOIDWavePhase GetWavePhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category="Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	UFUNCTION(BlueprintPure, Category="Wave")
	FVOIDWaveData GetCurrentWaveData() const { return CurrentWaveData; }

	// 차량 시동 성공 → 탈출 레벨로 전환
	UFUNCTION(BlueprintCallable, Category="Escape")
	void HandleEscapeSuccess(AActor* Driver);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Escape")
	FName EscapeLevelName = TEXT("Lv_Escape");

	// 플레이어 사망 → 게임오버 위젯 표시 또는 메인 메뉴로 전환
	UFUNCTION(BlueprintCallable, Category="GameOver")
	void HandleGameOver();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Escape")
	float EscapeTimeLimit = 600.0f; // 10분 = 600초

	UFUNCTION(BlueprintPure, Category="Escape")
	float GetEscapeRemaining() const { return EscapeRemaining; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameOver")
	FName GameOverLevelName = TEXT("Lv_Main");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameOver")
	float GameOverReturnDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameOver")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

protected:
	virtual void BeginPlay() override;

	void OnWaveTimerExpired();

	// 현재 WaveIndex와 일치하는 FloorIndex의 SpawnVolume에 SpawnWave() 호출
	void SpawnFloor(int32 FloorIndex);

	bool TryLoadWaveRow(int32 WaveIndex, FVOIDWaveData& OutData) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Wave")
	TObjectPtr<UDataTable> WaveDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 CurrentWaveIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	EVOIDWavePhase CurrentPhase = EVOIDWavePhase::Preparing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	FVOIDWaveData CurrentWaveData;

	FTimerHandle WaveTimerHandle;

	FTimerHandle EscapeTimerHandle;
	FTimerHandle EscapeTickHandle;
	float EscapeRemaining = 0.0f;

	void StartEscapeTimer();
	void OnEscapeTick();
	void OnEscapeTimerExpired();
};
