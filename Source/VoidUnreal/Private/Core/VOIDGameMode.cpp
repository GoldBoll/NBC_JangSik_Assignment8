#include "Core/VOIDGameMode.h"
#include "Core/VOIDGameState.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Waves/VOIDSpawnVolume.h"
#include "Items/VOIDPickupBase.h"
#include "Characters/VOIDZombieCharacter.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Core/VOIDPlayerController.h"
#include "UI/VOIDHUDWidget.h"

// 생성자 — Tick 비활성화 + GameState 클래스 지정
AVOIDGameMode::AVOIDGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	GameStateClass = AVOIDGameState::StaticClass();
}

// 시동 성공 → 모든 타이머 정리 후 Lv_Escape 로 전환
void AVOIDGameMode::HandleEscapeSuccess(AActor* Driver)
{
	UE_LOG(LogTemp, Display, TEXT("[VOID] HandleEscapeSuccess by %s → OpenLevel(%s)"),
		*GetNameSafe(Driver), *EscapeLevelName.ToString());

	GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
	GetWorldTimerManager().ClearTimer(EscapeTickHandle);

	UGameplayStatics::OpenLevel(this, EscapeLevelName);
}

// 게임오버 처리 — 타이머 정리 + HUD GameOver 모드 + 입력 차단
void AVOIDGameMode::HandleGameOver()
{
	UE_LOG(LogTemp, Display, TEXT("[VOID] HandleGameOver — show GameOver overlay (no level change)"));

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
	GetWorldTimerManager().ClearTimer(EscapeTickHandle);

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (auto* VPC = Cast<AVOIDPlayerController>(PC))
	{
		if (auto* HUD = Cast<UVOIDHUDWidget>(VPC->GetHUDWidgetInstance()))
		{
			HUD->SetHUDMode(EVOIDHUDMode::GameOver);
		}
	}
	if (PC)
	{
		PC->SetCinematicMode(true, false, false, true, true);
	}
}

// 게임 시작 — 1웨이브 시작 + 탈출 타이머 가동
void AVOIDGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartWave(1);
	StartEscapeTimer();
}

// 10분 탈출 타이머 + 1초 Tick 가동
void AVOIDGameMode::StartEscapeTimer()
{
	EscapeRemaining = EscapeTimeLimit;

	AVOIDGameState* GS = GetGameState<AVOIDGameState>();
	UE_LOG(LogTemp, Display, TEXT("[VOID] Escape timer start — %.0fs  GameState=%s"),
		EscapeRemaining, GS ? *GS->GetName() : TEXT("NULL (not AVOIDGameState!)"));

	if (GS)
	{
		GS->SetRemainingTime(EscapeRemaining);
	}

	GetWorldTimerManager().SetTimer(
		EscapeTimerHandle, this, &AVOIDGameMode::OnEscapeTimerExpired,
		EscapeTimeLimit, false);

	GetWorldTimerManager().SetTimer(
		EscapeTickHandle, this, &AVOIDGameMode::OnEscapeTick,
		1.0f, true);
}

// 1초마다 잔여시간 -1, GameState Broadcast → HUD TimeText 갱신
void AVOIDGameMode::OnEscapeTick()
{
	EscapeRemaining = FMath::Max(0.0f, EscapeRemaining - 1.0f);
	if (AVOIDGameState* GS = GetGameState<AVOIDGameState>())
	{
		GS->SetRemainingTime(EscapeRemaining);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[VOID] OnEscapeTick — GameState NULL, TimeText 미갱신"));
	}
}

void AVOIDGameMode::OnEscapeTimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("[VOID] Escape timer expired — GameOver"));
	GetWorldTimerManager().ClearTimer(EscapeTickHandle);
	HandleGameOver();
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
