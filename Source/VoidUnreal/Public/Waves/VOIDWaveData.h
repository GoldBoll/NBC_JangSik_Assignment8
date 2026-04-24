#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VOIDWaveData.generated.h"

UENUM(BlueprintType)
enum class EVOIDWaveEvent : uint8
{
	None        UMETA(DisplayName="없음"),
	Siren       UMETA(DisplayName="사이렌 (Wave 2)"),   // 주기적 광역 유혹
	CarRepair   UMETA(DisplayName="차량 수리 (Wave 3)") // 부품 수집·탈출
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
};
