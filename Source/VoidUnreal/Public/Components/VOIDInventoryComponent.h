#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType
#include "VOIDInventoryComponent.generated.h"

class UVOIDItemDataAsset;

// 무게 변경 시 HUD/디버프/이동속도가 구독하는 멀티캐스트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVOIDOnWeightChanged, float, CurrentWeight, float, MaxCarry);

// 단일 인벤토리 슬롯 — ItemData 1종 + 수량
USTRUCT(BlueprintType)
struct FVOIDInventorySlot
{
	GENERATED_BODY()

	// 슬롯에 담긴 아이템 종류 (DA_Part_Battery 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UVOIDItemDataAsset> ItemData;

	// 슬롯에 쌓인 수량 (MaxStackPerItem 상한)
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

	// 슬롯에서 수량만큼 차감 — 0이 되면 슬롯 제거 + 무게 갱신
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(UVOIDItemDataAsset* ItemData, int32 Quantity = 1);

	// 슬롯 타입에 맞는 첫 부품 DataAsset 반환 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, Category="Inventory")
	UVOIDItemDataAsset* FindPartByType(EVOIDVehiclePartType PartType) const;

	// 추가 무게 수용 가능 여부 (현재 + 추가 ≤ MaxCarry)
	UFUNCTION(BlueprintPure, Category="Inventory")
	bool CanCarry(float AdditionalWeight) const;

	// 현재 총 무게 조회
	UFUNCTION(BlueprintPure, Category="Inventory")
	float GetTotalWeight() const { return TotalWeight; }

	// 최대 운반 무게 조회
	UFUNCTION(BlueprintPure, Category="Inventory")
	float GetMaxCarry() const { return MaxCarry; }

	// WeightRatio = TotalWeight / MaxCarry (0.0 ~ 1.0+)
	UFUNCTION(BlueprintPure, Category="Inventory")
	float GetWeightRatio() const;

	// 인벤토리 전체 슬롯 배열 조회 (HUD에서 순회 표시용)
	const TArray<FVOIDInventorySlot>& GetSlots() const { return Slots; }

	// 무게 변경 이벤트 — HUD WeightBar / 디버프 Overweight 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnWeightChanged OnWeightChanged;

protected:
	// 최대 운반 무게 (kg)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	float MaxCarry = 30.0f; // kg

	// 같은 ItemData(=같은 종류) 1슬롯에 쌓을 수 있는 최대 개수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1"))
	int32 MaxStackPerItem = 2;

	// 누적 총 무게 (kg)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	float TotalWeight = 0.0f;

	// 인벤토리 슬롯 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FVOIDInventorySlot> Slots;

	// 모든 슬롯 무게 합산 → TotalWeight 갱신 + Broadcast
	void RecomputeTotalWeight();
};
