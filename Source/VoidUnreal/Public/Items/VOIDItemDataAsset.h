#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VOIDItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EVOIDItemRarity : uint8
{
	Common      UMETA(DisplayName="일반"),
	Uncommon    UMETA(DisplayName="고급"),
	Rare        UMETA(DisplayName="희귀"),
	Epic        UMETA(DisplayName="영웅")
};

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// 타르코프식 kg 단위 무게
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item", meta=(ClampMin="0.0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	int32 ScoreValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	EVOIDItemRarity Rarity = EVOIDItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	EVOIDItemCategory Category = EVOIDItemCategory::Misc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	int32 MaxStack = 1;
};
