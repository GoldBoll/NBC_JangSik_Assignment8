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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnDebuffUpdated, const TArray<EVOIDDebuffType>&, ActiveDebuffs);

USTRUCT(BlueprintType)
struct FVOIDActiveDebuff
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVOIDDebuffType Type = EVOIDDebuffType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude = 1.0f;
};

UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDDebuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDDebuffComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Debuff")
	void ApplyDebuff(EVOIDDebuffType Type, float Duration, float Magnitude = 1.0f);

	UFUNCTION(BlueprintCallable, Category="Debuff")
	void RemoveDebuff(EVOIDDebuffType Type);

	UFUNCTION(BlueprintPure, Category="Debuff")
	bool HasDebuff(EVOIDDebuffType Type) const;

	// 과적은 InventoryComponent 의 WeightRatio 로부터 매 Tick 계산 — 시간 기반이 아닌 상태 기반
	UFUNCTION(BlueprintCallable, Category="Debuff|Overweight")
	void UpdateOverweightFromInventory(float WeightRatio);

	UFUNCTION(BlueprintPure, Category="Debuff|Modifier")
	float GetMoveSpeedMultiplier() const;

	UFUNCTION(BlueprintPure, Category="Debuff|Modifier")
	float GetNoiseMultiplier() const;

	UFUNCTION(BlueprintPure, Category="Debuff|Modifier")
	float GetStaminaMultiplier() const;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnDebuffUpdated OnDebuffUpdated;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debuff")
	TArray<FVOIDActiveDebuff> ActiveDebuffs;

	// 과적은 무게 비율에서 파생되므로 별도 추적
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debuff|Overweight")
	float CurrentWeightRatio = 0.0f;

	void BroadcastUpdate();
};
