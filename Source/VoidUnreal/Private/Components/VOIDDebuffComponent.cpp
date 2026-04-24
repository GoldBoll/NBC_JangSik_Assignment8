#include "Components/VOIDDebuffComponent.h"

UVOIDDebuffComponent::UVOIDDebuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVOIDDebuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bChanged = false;
	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; --i)
	{
		// 과적은 무게 기반이라 시간 감소 없음
		if (ActiveDebuffs[i].Type == EVOIDDebuffType::Overweight) { continue; }

		ActiveDebuffs[i].RemainingTime -= DeltaTime;
		if (ActiveDebuffs[i].RemainingTime <= 0.0f)
		{
			ActiveDebuffs.RemoveAt(i);
			bChanged = true;
		}
	}

	if (bChanged) { BroadcastUpdate(); }
}

void UVOIDDebuffComponent::ApplyDebuff(EVOIDDebuffType Type, float Duration, float Magnitude)
{
	// 이미 있으면 지속시간 갱신 (중첩 X, 스택 X)
	for (FVOIDActiveDebuff& Debuff : ActiveDebuffs)
	{
		if (Debuff.Type == Type)
		{
			Debuff.RemainingTime = FMath::Max(Debuff.RemainingTime, Duration);
			Debuff.Magnitude = Magnitude;
			BroadcastUpdate();
			return;
		}
	}

	FVOIDActiveDebuff NewDebuff;
	NewDebuff.Type = Type;
	NewDebuff.RemainingTime = Duration;
	NewDebuff.Magnitude = Magnitude;
	ActiveDebuffs.Add(NewDebuff);
	BroadcastUpdate();
}

void UVOIDDebuffComponent::RemoveDebuff(EVOIDDebuffType Type)
{
	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; --i)
	{
		if (ActiveDebuffs[i].Type == Type)
		{
			ActiveDebuffs.RemoveAt(i);
			BroadcastUpdate();
			return;
		}
	}
}

bool UVOIDDebuffComponent::HasDebuff(EVOIDDebuffType Type) const
{
	for (const FVOIDActiveDebuff& Debuff : ActiveDebuffs)
	{
		if (Debuff.Type == Type) { return true; }
	}
	return false;
}

void UVOIDDebuffComponent::UpdateOverweightFromInventory(float WeightRatio)
{
	CurrentWeightRatio = WeightRatio;

	const bool bShouldApply = WeightRatio >= 0.5f;
	const bool bHasOverweight = HasDebuff(EVOIDDebuffType::Overweight);

	if (bShouldApply && !bHasOverweight)
	{
		ApplyDebuff(EVOIDDebuffType::Overweight, /*Duration*/ 0.0f, WeightRatio);
	}
	else if (!bShouldApply && bHasOverweight)
	{
		RemoveDebuff(EVOIDDebuffType::Overweight);
	}
	else if (bHasOverweight)
	{
		// 이미 과적 상태 — Magnitude만 갱신
		for (FVOIDActiveDebuff& Debuff : ActiveDebuffs)
		{
			if (Debuff.Type == EVOIDDebuffType::Overweight)
			{
				Debuff.Magnitude = WeightRatio;
				break;
			}
		}
	}
}

float UVOIDDebuffComponent::GetMoveSpeedMultiplier() const
{
	float Mult = 1.0f;
	if (HasDebuff(EVOIDDebuffType::Fracture))  { Mult *= 0.5f; }
	if (HasDebuff(EVOIDDebuffType::Overweight))
	{
		// 공식: 1.0 - (WeightRatio * 0.6)
		Mult *= FMath::Max(0.1f, 1.0f - CurrentWeightRatio * 0.6f);
	}
	return Mult;
}

float UVOIDDebuffComponent::GetNoiseMultiplier() const
{
	float Mult = 1.0f;
	if (HasDebuff(EVOIDDebuffType::Overweight))
	{
		// 공식: 1.0 + (WeightRatio * 1.2)
		Mult *= 1.0f + CurrentWeightRatio * 1.2f;
	}
	return Mult;
}

float UVOIDDebuffComponent::GetStaminaMultiplier() const
{
	float Mult = 1.0f;
	if (HasDebuff(EVOIDDebuffType::Overweight))
	{
		// 공식: 1.0 + (WeightRatio * 1.0)
		Mult *= 1.0f + CurrentWeightRatio;
	}
	return Mult;
}

void UVOIDDebuffComponent::BroadcastUpdate()
{
	TArray<EVOIDDebuffType> Types;
	for (const FVOIDActiveDebuff& Debuff : ActiveDebuffs)
	{
		Types.Add(Debuff.Type);
	}
	OnDebuffUpdated.Broadcast(Types);
}
