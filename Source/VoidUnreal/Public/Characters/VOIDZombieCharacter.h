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

	// 소음에 반응해 추적 개시 (NoiseComponent 브로드캐스트 → AI 수신)
	UFUNCTION(BlueprintCallable, Category="AI")
	void ReactToNoise(const FVector& NoiseLocation, float NoiseRadius);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float AttackDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float AttackRange = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float SightRange = 1200.0f;
};
