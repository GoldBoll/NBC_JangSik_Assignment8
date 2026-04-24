#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VOIDNoiseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnNoiseChanged, float, CurrentNoise);

UENUM(BlueprintType)
enum class EVOIDNoiseSource : uint8
{
	Footstep_Walk   UMETA(DisplayName="Walk"),
	Footstep_Run    UMETA(DisplayName="Run"),
	Gunshot         UMETA(DisplayName="Gunshot"),
	Melee           UMETA(DisplayName="Melee"),
	Pickup          UMETA(DisplayName="Pickup"),
	Reload          UMETA(DisplayName="Reload"),
	VehicleStart    UMETA(DisplayName="VehicleStart")
};

UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDNoiseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDNoiseComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Void 필러 "Noise is Currency" — 행동별 소음 발생
	// WeightMultiplier: 무게 기반 배수 (InventoryComponent와 연동)
	UFUNCTION(BlueprintCallable, Category="Noise")
	void EmitNoise(EVOIDNoiseSource Source, float WeightMultiplier = 1.0f);

	UFUNCTION(BlueprintPure, Category="Noise")
	float GetCurrentNoise() const { return CurrentNoise; }

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnNoiseChanged OnNoiseChanged;

protected:
	// 프레임당 감쇠량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Noise")
	float NoiseDecayPerSecond = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Noise")
	float CurrentNoise = 0.0f;

	// 좀비 AI에 청각 이벤트 브로드캐스트 (AIPerceptionSystem 우회 가능)
	void BroadcastNoise(float Radius);
};
