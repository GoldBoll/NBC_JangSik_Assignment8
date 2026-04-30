#pragma once

#include "CoreMinimal.h"
#include "Characters/VOIDBaseCharacter.h"
#include "VOIDZombieCharacter.generated.h"

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDZombieCharacter : public AVOIDBaseCharacter
{
	GENERATED_BODY()

public:
	AVOIDZombieCharacter();

protected:
	virtual void BeginPlay() override;

	// 좀비 사망 처리 — GameState에 점수 가산 + 액터 파괴 예약
	UFUNCTION()
	void HandleZombieDeath();

	// 처치 시 플레이어에게 부여할 점수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Score")
	int32 ScoreReward = 10;

	// 공격 한 번에 입히는 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float AttackDamage = 10.0f;

	// 공격 사거리 (이내일 때 AttackPlayer 호출)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float AttackRange = 100.0f;

	// 시야 거리 (AIPerception 시각/Trace 한계)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float SightRange = 1200.0f;


	//Combat
public:
	// 사거리 안의 플레이어에게 데미지 적용 (BT의 Attack Task에서 호출)
	UFUNCTION(BlueprintCallable, Category="Combat")
	void AttackPlayer(AActor* Target);

protected:
	bool bCanAttack = true;                  // 쿨다운 중이 아닐 때 true
	FTimerHandle AttackCooldownHandle;       // 공격 쿨다운 타이머 핸들

	// 공격 사이 쿨다운(초)
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackCooldown = 1.0f;
};
