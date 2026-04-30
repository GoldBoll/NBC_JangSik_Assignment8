#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VOIDBaseCharacter.generated.h"

class UVOIDHealthComponent;

UCLASS(Blueprintable, Abstract)
class VOIDUNREAL_API AVOIDBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVOIDBaseCharacter();

	// 데미지 적용 — HealthComponent로 위임 (플레이어/좀비 공통)
	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual void ApplyDamage(float DamageAmount);

	// 체력 0 여부 판정 (HealthComponent 위임)
	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsDead() const;

protected:
	virtual void BeginPlay() override;

	// 체력 관리 컴포넌트 (BaseCharacter가 기본 장착)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDHealthComponent> HealthComponent;
};
