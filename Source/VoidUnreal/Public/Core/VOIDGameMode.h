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
};
