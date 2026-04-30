#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType
#include "VOIDItemDataAsset.generated.h"

// 아이템 희귀도 — 점수/스폰 확률에 영향
UENUM(BlueprintType)
enum class EVOIDItemRarity : uint8
{
	Common      UMETA(DisplayName="일반"),
	Uncommon    UMETA(DisplayName="고급"),
	Rare        UMETA(DisplayName="희귀"),
	Epic        UMETA(DisplayName="영웅")
};

// 아이템 카테고리 분류
UENUM(BlueprintType)
enum class EVOIDItemCategory : uint8
{
	None        UMETA(Hidden),
	Consumable  UMETA(DisplayName="소비 아이템"),
	Weapon      UMETA(DisplayName="무기"),
	Ammo        UMETA(DisplayName="탄약"),
	VehiclePart UMETA(DisplayName="차량 부품"),
	Misc        UMETA(DisplayName="기타")
};

UCLASS(BlueprintType)
class VOIDUNREAL_API UVOIDItemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 고유 식별자 (예: Battery, Bandage, RifleAmmo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FName ItemID;

	// UI 표시 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FText DisplayName;

	// 툴팁 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FText Description;

	// HUD 인벤토리 아이콘 (선택)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// 타르코프식 kg 단위 무게
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item", meta=(ClampMin="0.0"))
	float Weight = 1.0f;

	// 획득 시 점수 가산값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	int32 ScoreValue = 0;

	// 희귀도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	EVOIDItemRarity Rarity = EVOIDItemRarity::Common;

	// 카테고리 (소비/무기/탄약/차량부품/기타)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	EVOIDItemCategory Category = EVOIDItemCategory::Misc;

	// 슬롯당 최대 스택 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	int32 MaxStack = 1;

	// Category==VehiclePart 일 때만 의미 있음 (슬롯 매칭용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item|VehiclePart")
	EVOIDVehiclePartType PartType = EVOIDVehiclePartType::None;
};
