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

	UFUNCTION()
	void HandleZombieDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Score")
	int32 ScoreReward = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float AttackDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float AttackRange = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	float SightRange = 1200.0f;
	
	
	//Combat
public:
	UFUNCTION(BlueprintCallable, Category="Combat")
	void AttackPlayer(AActor* Target);

protected:
	bool bCanAttack = true;
	FTimerHandle AttackCooldownHandle;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackCooldown = 1.0f;
};
