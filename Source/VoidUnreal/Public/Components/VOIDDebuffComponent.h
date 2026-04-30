#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VOIDDebuffComponent.generated.h"

UENUM(BlueprintType)
enum class EVOIDDebuffType : uint8
{
	None          UMETA(Hidden),
	Bleeding      UMETA(DisplayName="출혈"),   // 체력 -1/s
	Fracture      UMETA(DisplayName="골절"),   // 이속 -50%
	Overweight    UMETA(DisplayName="과적")    // 무게 기반 동적 페널티
};

// 활성 디버프 목록 변경 시 HUD가 구독하는 멀티캐스트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnDebuffUpdated, const TArray<EVOIDDebuffType>&, ActiveDebuffs);

// 활성화된 단일 디버프 인스턴스 (종류 + 잔여시간 + 크기)
USTRUCT(BlueprintType)
struct FVOIDActiveDebuff
{
	GENERATED_BODY()

	// 디버프 종류 (출혈/골절/과적)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVOIDDebuffType Type = EVOIDDebuffType::None;

	// 남은 지속 시간 (초). Tick으로 감소
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingTime = 0.0f;

	// 효과 크기 (출혈 데미지/이속 패널티 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 1.0f;
};

UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDDebuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDDebuffComponent();

	// 매 프레임 디버프 잔여시간 감소 + 출혈 데미지 누적 처리
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 디버프 부여 — 같은 종류면 잔여시간 갱신
	UFUNCTION(BlueprintCallable, Category="Debuff")
	void ApplyDebuff(EVOIDDebuffType Type, float Duration, float Magnitude = 1.0f);

	// 특정 디버프 제거 + Broadcast
	UFUNCTION(BlueprintCallable, Category="Debuff")
	void RemoveDebuff(EVOIDDebuffType Type);

	// 특정 디버프 활성 여부
	UFUNCTION(BlueprintPure, Category="Debuff")
	bool HasDebuff(EVOIDDebuffType Type) const;

	// 과적은 InventoryComponent 의 WeightRatio 로부터 매 Tick 계산 — 시간 기반이 아닌 상태 기반
	UFUNCTION(BlueprintCallable, Category="Debuff|Overweight")
	void UpdateOverweightFromInventory(float WeightRatio);

	// 이동속도 곱셈 (과적/골절 합산)
	UFUNCTION(BlueprintPure, Category="Debuff|Modifier")
	float GetMoveSpeedMultiplier() const;

	// 소음 곱셈 (과적이 소음을 키움)
	UFUNCTION(BlueprintPure, Category="Debuff|Modifier")
	float GetNoiseMultiplier() const;

	// 스태미나 소모 곱셈
	UFUNCTION(BlueprintPure, Category="Debuff|Modifier")
	float GetStaminaMultiplier() const;

	// 디버프 목록 변경 이벤트 — HUD DebuffText 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnDebuffUpdated OnDebuffUpdated;

protected:
	// 활성화된 디버프 목록 (시간 기반)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debuff")
	TArray<FVOIDActiveDebuff> ActiveDebuffs;

	// 과적은 무게 비율에서 파생되므로 별도 추적
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debuff|Overweight")
	float CurrentWeightRatio = 0.0f;

	// 활성 디버프 종류만 추출해서 Broadcast
	void BroadcastUpdate();
};
