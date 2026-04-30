#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VOIDHealthComponent.generated.h"

// 체력 변경 / 사망 시 HUD·디버프·GameMode가 구독하는 멀티캐스트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVOIDOnDeath);

UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDHealthComponent();

	// 데미지 적용 — 체력 감소 + Broadcast, 0 이하 시 OnDeath 발동
	UFUNCTION(BlueprintCallable, Category="Health")
	void ApplyDamage(float Amount);

	// 체력 회복 (MaxHealth 상한 클램프)
	UFUNCTION(BlueprintCallable, Category="Health")
	void Heal(float Amount);

	// 현재 체력 조회
	UFUNCTION(BlueprintPure, Category="Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	// 최대 체력 조회
	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const { return MaxHealth; }

	// 사망 여부 (체력 0 이하)
	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return CurrentHealth <= 0.0f; }

	// 체력 변경 이벤트 — HUD HealthBar 구독
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnHealthChanged OnHealthChanged;

	// 사망 이벤트 — PlayerController/좀비가 구독해 후처리 위임
	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

	// 최대 체력 (BP에서 설정 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100.0f;

	// 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float CurrentHealth = 100.0f;
};
