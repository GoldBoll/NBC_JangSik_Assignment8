#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VOIDNoiseComponent.generated.h"

// 소음 변경 시 HUD 게이지가 구독하는 멀티캐스트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnNoiseChanged, float, CurrentNoise);

// 소음 발생원 종류 — 각 소스별 BaseNoise/Radius 매핑
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

	// 매 프레임 NoiseDecayPerSecond만큼 감쇠
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 행동별 소음 발생 — WeightMultiplier는 무게 기반 배수
	UFUNCTION(BlueprintCallable, Category="Noise")
	void EmitNoise(EVOIDNoiseSource Source, float WeightMultiplier = 1.0f);

	// 현재 소음 수치 조회 (HUD NoiseBar 바인딩용)
	UFUNCTION(BlueprintPure, Category="Noise")
	float GetCurrentNoise() const { return CurrentNoise; }

	// 발신원 층 (1=로비, 2=거주, 3=옥상)
	UFUNCTION(BlueprintCallable, Category="Noise")
	void SetFloorIndex(int32 NewIndex) { OwnerFloorIndex = NewIndex; }

	UFUNCTION(BlueprintPure, Category="Noise")
	int32 GetFloorIndex() const { return OwnerFloorIndex; }

	// 소음 변경 이벤트 — HUD 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnNoiseChanged OnNoiseChanged;

protected:
	// 프레임당 감쇠량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Noise")
	float NoiseDecayPerSecond = 2.0f;

	// 발신원 층 인덱스 (1=로비, 2=거주, 3=옥상)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Noise", meta=(ClampMin="1", ClampMax="3"))
	int32 OwnerFloorIndex = 1;

	// 한 층 차이당 감쇠 계수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Noise", meta=(ClampMin="0.0", ClampMax="1.0"))
	float PerFloorAttenuation = 0.4f;

	// 누적 소음값 (감쇠 적용됨)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Noise")
	float CurrentNoise = 0.0f;

	// 좀비 AI에 청각 이벤트 브로드캐스트 (AIPerceptionSystem 우회 가능)
	void BroadcastNoise(float Radius);
};
