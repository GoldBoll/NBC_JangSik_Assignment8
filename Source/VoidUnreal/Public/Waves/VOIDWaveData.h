#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Templates/SubclassOf.h"
#include "VOIDWaveData.generated.h"

class AVOIDZombieCharacter;
class AVOIDPickupBase;

UENUM(BlueprintType)
enum class EVOIDWaveEvent : uint8
{
	None        UMETA(DisplayName="없음"),
	Siren       UMETA(DisplayName="사이렌 (Wave 2)"),   // 주기적 광역 유혹
	CarRepair   UMETA(DisplayName="차량 수리 (Wave 3)") // 부품 수집·탈출
};

UENUM(BlueprintType)
enum class EVOIDFloorGoal : uint8
{
	Loot     UMETA(DisplayName="파밍 (1층)"),
	Combat   UMETA(DisplayName="전투 돌파 (2층)"),
	Repair   UMETA(DisplayName="차량 수리·탈출 (3층)")
};

USTRUCT(BlueprintType)
struct FVOIDWaveData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveIndex = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText WaveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0"))
	float TimeLimit = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	int32 ZombieCount = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	int32 ItemSpawnCount = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVOIDWaveEvent EventType = EVOIDWaveEvent::None;

	// 좀비 이동속도 배율 (Wave 진행에 따라 증가)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1"))
	float ZombieSpeedMultiplier = 1.0f;

	// 좀비 시야 거리 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.1"))
	float ZombieSightMultiplier = 1.0f;

	// UI 배너 표기용 층 테마 (예: "1층 — 로비")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText FloorTheme;

	// 층 클리어 목표
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVOIDFloorGoal GoalType = EVOIDFloorGoal::Loot;

	// 이 층에 스폰할 좀비 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AVOIDZombieCharacter> ZombieClass;

	// 이 층에서 사용할 픽업 풀 (랜덤 추첨, 비우면 아이템 스폰 안 됨)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AVOIDPickupBase>> PickupPool;
};
