#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType
#include "VOIDInventoryComponent.generated.h"

class UVOIDItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVOIDOnWeightChanged, float, CurrentWeight, float, MaxCarry);

USTRUCT(BlueprintType)
struct FVOIDInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UVOIDItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;
};

UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDInventoryComponent();

	// 타르코프식 정밀 무게 — 1kg 단위, 0.1kg 이하 허용
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TryAddItem(UVOIDItemDataAsset* ItemData, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(UVOIDItemDataAsset* ItemData, int32 Quantity = 1);

	// 슬롯 타입에 맞는 첫 부품 DataAsset 반환 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, Category="Inventory")
	UVOIDItemDataAsset* FindPartByType(EVOIDVehiclePartType PartType) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	bool CanCarry(float AdditionalWeight) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	float GetTotalWeight() const { return TotalWeight; }

	UFUNCTION(BlueprintPure, Category="Inventory")
	float GetMaxCarry() const { return MaxCarry; }

	// WeightRatio = TotalWeight / MaxCarry (0.0 ~ 1.0+)
	UFUNCTION(BlueprintPure, Category="Inventory")
	float GetWeightRatio() const;

	const TArray<FVOIDInventorySlot>& GetSlots() const { return Slots; }

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnWeightChanged OnWeightChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	float MaxCarry = 30.0f; // kg

	// 같은 ItemData(=같은 종류) 1슬롯에 쌓을 수 있는 최대 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1"))
	int32 MaxStackPerItem = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	float TotalWeight = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FVOIDInventorySlot> Slots;

	void RecomputeTotalWeight();
};
