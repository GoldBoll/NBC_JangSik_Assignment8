#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VOIDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVOIDOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVOIDOnDeath);

UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDHealthComponent();

	UFUNCTION(BlueprintCallable, Category="Health")
	void ApplyDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category="Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintPure, Category="Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return CurrentHealth <= 0.0f; }

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FVOIDOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float CurrentHealth = 100.0f;
};
