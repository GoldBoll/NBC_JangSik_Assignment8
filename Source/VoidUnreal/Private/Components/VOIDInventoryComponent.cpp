#include "Components/VOIDInventoryComponent.h"
#include "Items/VOIDItemDataAsset.h"

UVOIDInventoryComponent::UVOIDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UVOIDInventoryComponent::TryAddItem(UVOIDItemDataAsset* ItemData, int32 Quantity)
{
	if (!ItemData || Quantity <= 0) { return false; }

	// 동일 ItemData 슬롯이 이미 있으면 누적 — 단, MaxStackPerItem 까지만
	for (FVOIDInventorySlot& Existing : Slots)
	{
		if (Existing.ItemData == ItemData)
		{
			const int32 Allowed = FMath::Max(0, MaxStackPerItem - Existing.Quantity);
			if (Allowed <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Inventory] %s 스택 가득 (%d/%d) — 픽업 거부"),
					*ItemData->GetName(), Existing.Quantity, MaxStackPerItem);
				return false;
			}
			const int32 ActualAdd = FMath::Min(Quantity, Allowed);
			const float AddedWeight = ItemData->Weight * ActualAdd;
			if (!CanCarry(AddedWeight)) { return false; }

			Existing.Quantity += ActualAdd;
			RecomputeTotalWeight();
			return true;
		}
	}

	const int32 ActualAdd = FMath::Min(Quantity, MaxStackPerItem);
	const float AddedWeight = ItemData->Weight * ActualAdd;
	if (!CanCarry(AddedWeight)) { return false; }

	FVOIDInventorySlot NewSlot;
	NewSlot.ItemData = ItemData;
	NewSlot.Quantity = ActualAdd;
	Slots.Add(NewSlot);

	RecomputeTotalWeight();
	return true;
}

// 슬롯에서 Quantity 차감, 0 이하면 슬롯 제거 후 무게 재산출
bool UVOIDInventoryComponent::RemoveItem(UVOIDItemDataAsset* ItemData, int32 Quantity)
{
	if (!ItemData || Quantity <= 0) { return false; }

	for (int32 i = Slots.Num() - 1; i >= 0; --i)
	{
		if (Slots[i].ItemData == ItemData)
		{
			Slots[i].Quantity -= Quantity;
			if (Slots[i].Quantity <= 0)
			{
				Slots.RemoveAt(i);
			}
			RecomputeTotalWeight();
			return true;
		}
	}
	return false;
}

// VehiclePart 카테고리 중 PartType 일치 슬롯의 ItemData 반환, 없으면 nullptr
UVOIDItemDataAsset* UVOIDInventoryComponent::FindPartByType(EVOIDVehiclePartType PartType) const
{
	for (const FVOIDInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData
			&& Slot.ItemData->Category == EVOIDItemCategory::VehiclePart
			&& Slot.ItemData->PartType == PartType
			&& Slot.Quantity > 0)
		{
			return Slot.ItemData;
		}
	}
	return nullptr;
}

// 추가 무게 포함 시 MaxCarry 초과 여부
bool UVOIDInventoryComponent::CanCarry(float AdditionalWeight) const
{
	return (TotalWeight + AdditionalWeight) <= MaxCarry;
}

// TotalWeight / MaxCarry 비율 (0~1)
float UVOIDInventoryComponent::GetWeightRatio() const
{
	return MaxCarry > 0.0f ? (TotalWeight / MaxCarry) : 0.0f;
}

// 슬롯 전체 무게 합산 후 OnWeightChanged Broadcast
void UVOIDInventoryComponent::RecomputeTotalWeight()
{
	float Sum = 0.0f;
	for (const FVOIDInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData)
		{
			Sum += Slot.ItemData->Weight * Slot.Quantity;
		}
	}
	TotalWeight = Sum;
	OnWeightChanged.Broadcast(TotalWeight, MaxCarry);
}
