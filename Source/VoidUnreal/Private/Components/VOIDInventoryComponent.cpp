#include "Components/VOIDInventoryComponent.h"
#include "Items/VOIDItemDataAsset.h"

UVOIDInventoryComponent::UVOIDInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UVOIDInventoryComponent::TryAddItem(UVOIDItemDataAsset* ItemData, int32 Quantity)
{
	if (!ItemData || Quantity <= 0) { return false; }

	const float AddedWeight = ItemData->Weight * Quantity;
	if (!CanCarry(AddedWeight)) { return false; }

	FVOIDInventorySlot NewSlot;
	NewSlot.ItemData = ItemData;
	NewSlot.Quantity = Quantity;
	Slots.Add(NewSlot);

	RecomputeTotalWeight();
	return true;
}

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

bool UVOIDInventoryComponent::CanCarry(float AdditionalWeight) const
{
	return (TotalWeight + AdditionalWeight) <= MaxCarry;
}

float UVOIDInventoryComponent::GetWeightRatio() const
{
	return MaxCarry > 0.0f ? (TotalWeight / MaxCarry) : 0.0f;
}

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
