#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Waves/VOIDWaveData.h"
#include "VOIDGameMode.generated.h"

class UDataTable;

// 웨이브 진행 상태 머신 (스폰 대기 → 진행 → 종료 처리 → 완료)
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

	// 웨이브 시작 — DataTable에서 행 로드 + SpawnVolume 호출 + 타이머 가동
	UFUNCTION(BlueprintCallable, Category="Wave")
	void StartWave(int32 WaveIndex);

	// 현재 웨이브 정상 종료 처리 (타이머 정리 + Phase Ending)
	UFUNCTION(BlueprintCallable, Category="Wave")
	void EndWave();

	// 다음 웨이브로 진행 (FloorTransitionTrigger에서 호출)
	UFUNCTION(BlueprintCallable, Category="Wave")
	void AdvanceWave();

	// 현재 웨이브 진행 단계 조회
	UFUNCTION(BlueprintPure, Category="Wave")
	EVOIDWavePhase GetWavePhase() const { return CurrentPhase; }

	// 현재 웨이브 인덱스(=Floor) 조회
	UFUNCTION(BlueprintPure, Category="Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	// 현재 웨이브의 DataTable 행 사본 반환
	UFUNCTION(BlueprintPure, Category="Wave")
	FVOIDWaveData GetCurrentWaveData() const { return CurrentWaveData; }

	// 차량 시동 성공 → 탈출 레벨로 전환
	UFUNCTION(BlueprintCallable, Category="Escape")
	void HandleEscapeSuccess(AActor* Driver);

	// 차량 시동 성공 시 OpenLevel로 전환할 탈출 레벨 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Escape")
	FName EscapeLevelName = TEXT("Lv_Escape");

	// 플레이어 사망 → 게임오버 위젯 표시 또는 메인 메뉴로 전환
	UFUNCTION(BlueprintCallable, Category="GameOver")
	void HandleGameOver();

	// 탈출 제한 시간 (10분)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Escape")
	float EscapeTimeLimit = 600.0f; // 10분 = 600초

	// 탈출 타이머 잔여 시간 조회 (HUD 바인딩용)
	UFUNCTION(BlueprintPure, Category="Escape")
	float GetEscapeRemaining() const { return EscapeRemaining; }

	// 게임오버 후 자동 전환할 레벨 (보통 메인 메뉴)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameOver")
	FName GameOverLevelName = TEXT("Lv_Main");

	// 게임오버 위젯 표시 후 레벨 전환까지 대기 시간(초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameOver")
	float GameOverReturnDelay = 3.0f;

	// 게임오버 시 viewport에 추가할 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameOver")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

protected:
	virtual void BeginPlay() override;

	// 웨이브 타이머 만료 시 호출 — 다음 웨이브 진행
	void OnWaveTimerExpired();

	// 현재 WaveIndex와 일치하는 FloorIndex의 SpawnVolume에 SpawnWave() 호출
	void SpawnFloor(int32 FloorIndex);

	// DataTable에서 WaveIndex 행을 찾아 OutData에 채움 (실패 시 false)
	bool TryLoadWaveRow(int32 WaveIndex, FVOIDWaveData& OutData) const;

	// 웨이브별 시간/좀비/아이템 정보 DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Wave")
	TObjectPtr<UDataTable> WaveDataTable;

	// 현재 진행 중인 웨이브 번호 (1=1F, 2=2F, 3=3F)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 CurrentWaveIndex = 0;

	// 현재 웨이브 진행 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	EVOIDWavePhase CurrentPhase = EVOIDWavePhase::Preparing;

	// 현재 웨이브 데이터 사본 (DataTable에서 로드)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	FVOIDWaveData CurrentWaveData;

	// 웨이브 종료 타이머 핸들
	FTimerHandle WaveTimerHandle;

	FTimerHandle EscapeTimerHandle;        // 탈출 만료 타이머
	FTimerHandle EscapeTickHandle;         // 1초 단위 탈출 카운트다운 핸들
	float EscapeRemaining = 0.0f;          // 탈출 잔여 시간(초)

	// 탈출 타이머 가동 (BeginPlay에서 호출)
	void StartEscapeTimer();
	// 1초마다 EscapeRemaining 감소 + GameState Broadcast
	void OnEscapeTick();
	// 탈출 시간 만료 — 자동 게임오버
	void OnEscapeTimerExpired();
};
