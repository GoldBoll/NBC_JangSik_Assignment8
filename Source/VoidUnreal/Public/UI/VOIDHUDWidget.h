#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VOIDDebuffComponent.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType
#include "VOIDHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UWidget;
class UButton;

UENUM(BlueprintType)
enum class EVOIDHUDMode : uint8
{
	MainMenu,
	InGame,
	GameOver,
	GameClear
};
class UVOIDHealthComponent;
class UVOIDInventoryComponent;
class UVOIDNoiseComponent;
class UVOIDDebuffComponent;
class UVOIDWeaponConfig;
class AVOIDVehicle;

UCLASS()
class VOIDUNREAL_API UVOIDHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="HUD")
	void BindToPlayer(APawn* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void BindToVehicle(AVOIDVehicle* Vehicle);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetHUDMode(EVOIDHUDMode NewMode);

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

	UFUNCTION()
	void HandleWeaponEquipped(UVOIDWeaponConfig* NewWeapon);

	void RefreshInventoryText();

	UFUNCTION()
	void HandleSlotInstalled(EVOIDVehiclePartType PartType);

	UFUNCTION()
	void HandleRepairComplete();

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

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Inventory;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> WeaponIcon;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> RepairText;

	// — 레벨별 페이지 컨테이너 —
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> Lv_1; // 메뉴/게임오버/게임클리어 공용 패널

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> Lv_2; // 인게임 HUD

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWidget> Lv_3;

	// — Lv_1 안 버튼 —
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Start;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> ReStart;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Exit;

	// — Lv_1 안 텍스트 —
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> GameOverText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> GameOverText_1; // BP 트리상 GameClear 표기

	// — 레벨 이름 매핑 —
	UPROPERTY(EditDefaultsOnly, Category="HUD|Pages")
	FName MenuLevelName = TEXT("Lv_Main");

	UPROPERTY(EditDefaultsOnly, Category="HUD|Pages")
	FName GameLevelName = TEXT("Lv_VoidProto");

	UPROPERTY(EditDefaultsOnly, Category="HUD|Pages")
	FName EscapeLevelName = TEXT("Lv_Escape");

	// — 버튼 핸들러 —
	UFUNCTION()
	void OnClickStart();

	UFUNCTION()
	void OnClickRestart();

	UFUNCTION()
	void OnClickExit();

private:
	TWeakObjectPtr<APawn> BoundPawn;
	TWeakObjectPtr<AVOIDVehicle> BoundVehicle;
	int32 InstalledCountCached = 0;
};
