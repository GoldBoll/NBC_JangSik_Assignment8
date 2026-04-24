#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VOIDDebuffComponent.h"
#include "VOIDHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UVOIDHealthComponent;
class UVOIDInventoryComponent;
class UVOIDNoiseComponent;
class UVOIDDebuffComponent;

UCLASS()
class VOIDUNREAL_API UVOIDHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="HUD")
	void BindToPlayer(APawn* PlayerPawn);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleHealthChanged(float NewHealth);

	UFUNCTION()
	void HandleWeightChanged(float TotalWeight, float MaxCarry);

	UFUNCTION()
	void HandleNoiseChanged(float NoiseLevel);

	UFUNCTION()
	void HandleDebuffUpdated(const TArray<EVOIDDebuffType>& ActiveDebuffs);

	UFUNCTION()
	void HandleScoreChanged(int32 NewScore);

	UFUNCTION()
	void HandleWaveChanged(int32 NewWave);

	UFUNCTION()
	void HandleTimeChanged(float RemainingTime);

	// — BindWidget —
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> WeightBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> NoiseBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WaveText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DebuffText;

private:
	TWeakObjectPtr<APawn> BoundPawn;
};
