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

	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsDead() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDHealthComponent> HealthComponent;
};
