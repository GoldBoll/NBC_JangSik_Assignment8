#include "Components/VOIDNoiseComponent.h"

#include "Perception/AISense_Hearing.h"
#include "DrawDebugHelpers.h"

UVOIDNoiseComponent::UVOIDNoiseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVOIDNoiseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentNoise > 0.0f)
	{
		CurrentNoise = FMath::Max(0.0f, CurrentNoise - NoiseDecayPerSecond * DeltaTime);
		OnNoiseChanged.Broadcast(CurrentNoise);
	}
}

void UVOIDNoiseComponent::EmitNoise(EVOIDNoiseSource Source, float WeightMultiplier)
{
	float BaseNoise = 0.0f;
	float Radius = 0.0f;
	switch (Source)
	{
	case EVOIDNoiseSource::Footstep_Walk: BaseNoise = 1.0f;  Radius = 300.0f;  break;
	case EVOIDNoiseSource::Footstep_Run:  BaseNoise = 3.0f;  Radius = 800.0f;  break;
	case EVOIDNoiseSource::Gunshot:       BaseNoise = 10.0f; Radius = 3000.0f; break;
	case EVOIDNoiseSource::Melee:         BaseNoise = 2.0f;  Radius = 500.0f;  break;
	case EVOIDNoiseSource::Pickup:        BaseNoise = 0.5f;  Radius = 200.0f;  break;
	case EVOIDNoiseSource::Reload:        BaseNoise = 1.5f;  Radius = 400.0f;  break;
	case EVOIDNoiseSource::VehicleStart:  BaseNoise = 15.0f; Radius = 5000.0f; break;
	default: break;
	}

	const float Scaled = BaseNoise * WeightMultiplier;
	CurrentNoise = FMath::Min(CurrentNoise + Scaled, 100.0f);
	OnNoiseChanged.Broadcast(CurrentNoise);

	// TODO: 청자 층 주입 시 FloorDiff 기반 감쇠 활성화
	const int32 ListenerFloor = OwnerFloorIndex;
	const int32 FloorDiff = ListenerFloor - OwnerFloorIndex;
	const float Attenuation = (FloorDiff > 0) ? FMath::Pow(PerFloorAttenuation, FloorDiff) : 1.f;

	BroadcastNoise(Radius * WeightMultiplier * Attenuation);
}

void UVOIDNoiseComponent::BroadcastNoise(float Radius)
{
	if (!GetOwner() || !GetWorld()) return;

	const FVector NoiseLocation = GetOwner()->GetActorLocation();

	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(), NoiseLocation, 1.0f, GetOwner(), Radius, NAME_None);

#if !(UE_BUILD_SHIPPING)
	DrawDebugSphere(GetWorld(), NoiseLocation, Radius, 16, FColor::Cyan, false, 1.0f, 0, 1.5f);
#endif
}
