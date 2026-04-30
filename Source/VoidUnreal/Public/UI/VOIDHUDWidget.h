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

// HUD 4페이지 모드 — 레벨명에서 자동 결정 + Lv_1/Lv_2 가시성 토글
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
	// 플레이어 폰의 컴포넌트 4종(Health/Inventory/Noise/Debuff/Weapon) 델리게이트 자동 바인딩
	UFUNCTION(BlueprintCallable, Category="HUD")
	void BindToPlayer(APawn* PlayerPawn);

	// 차량 OnSlotInstalled / OnRepairComplete 델리게이트 바인딩 (RepairText용)
	UFUNCTION(BlueprintCallable, Category="HUD")
	void BindToVehicle(AVOIDVehicle* Vehicle);

	// 4페이지 모드 전환 — 가시성 + 마우스 + 입력 모드 일괄 토글
	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetHUDMode(EVOIDHUDMode NewMode);

protected:
	// 위젯 생성 후 — 레벨명으로 모드 자동 결정 + GameState 델리게이트 구독 + 버튼 자동 바인딩
	virtual void NativeConstruct() override;

	// 체력 변동 시 HealthBar 갱신
	UFUNCTION()
	void HandleHealthChanged(float NewHealth);

	// 무게 변동 시 WeightBar + 인벤토리 텍스트 갱신
	UFUNCTION()
	void HandleWeightChanged(float TotalWeight, float MaxCarry);

	// 소음 변동 시 NoiseBar 갱신
	UFUNCTION()
	void HandleNoiseChanged(float NoiseLevel);

	// 활성 디버프 목록 갱신 → DebuffText 갱신
	UFUNCTION()
	void HandleDebuffUpdated(const TArray<EVOIDDebuffType>& ActiveDebuffs);

	// 점수 변동 시 ScoreText 갱신
	UFUNCTION()
	void HandleScoreChanged(int32 NewScore);

	// 웨이브 변경 시 WaveText 갱신
	UFUNCTION()
	void HandleWaveChanged(int32 NewWave);

	// 잔여 시간 1초 주기 갱신 → TimeText "MM:SS"
	UFUNCTION()
	void HandleTimeChanged(float RemainingTime);

	// 무기 장착 시 WeaponIcon / WeaponNameText 갱신
	UFUNCTION()
	void HandleWeaponEquipped(UVOIDWeaponConfig* NewWeapon);

	// 인벤토리 텍스트 슬롯 새로고침 (HandleWeightChanged에서 호출)
	void RefreshInventoryText();

	// 슬롯 설치 시 RepairText "0/3 → 1/3 ..." 갱신
	UFUNCTION()
	void HandleSlotInstalled(EVOIDVehiclePartType PartType);

	// 모든 슬롯 설치 완료 — RepairText 색상/문구 변경 ("Press E to Start")
	UFUNCTION()
	void HandleRepairComplete();

	// — BindWidget — (BP에서 같은 이름 위젯 필수)

	// 체력 게이지
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	// 무게 게이지 (WeightRatio)
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> WeightBar;

	// 소음 게이지
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> NoiseBar;

	// 점수 텍스트
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	// 웨이브 텍스트 ("Wave 1")
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WaveText;

	// 시간 텍스트 ("MM:SS")
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TimeText;

	// 활성 디버프 표시 ("[출혈] [골절]")
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DebuffText;

	// 인벤토리 슬롯 요약 텍스트 (선택)
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Inventory;

	// 무기 아이콘 이미지 (선택)
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> WeaponIcon;

	// 무기 이름 텍스트 (선택)
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> WeaponNameText;

	// 차량 수리 진행도 텍스트 (선택)
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

	// 메인 메뉴 시작 버튼
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Start;

	// 게임오버 시 재시작 버튼
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> ReStart;

	// 종료 버튼 (메뉴/게임오버/게임클리어 공용)
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Exit;

	// — Lv_1 안 텍스트 —

	// 게임오버 헤더 텍스트
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> GameOverText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> GameOverText_1; // BP 트리상 GameClear 표기

	// — 레벨 이름 매핑 —

	// 메인 메뉴 레벨 (Start 버튼이 → 게임 레벨로 OpenLevel)
	UPROPERTY(EditDefaultsOnly, Category="HUD|Pages")
	FName MenuLevelName = TEXT("Lv_Main");

	// 인게임 레벨 (NativeConstruct에서 InGame 모드 자동 진입)
	UPROPERTY(EditDefaultsOnly, Category="HUD|Pages")
	FName GameLevelName = TEXT("Lv_VoidProto");

	// 탈출 엔딩 레벨 (NativeConstruct에서 GameClear 모드 자동 진입)
	UPROPERTY(EditDefaultsOnly, Category="HUD|Pages")
	FName EscapeLevelName = TEXT("Lv_Escape");

	// — 버튼 핸들러 —

	// Start 버튼 클릭 — 게임 레벨로 OpenLevel
	UFUNCTION()
	void OnClickStart();

	// ReStart 버튼 클릭 — 현재 레벨 다시 로드
	UFUNCTION()
	void OnClickRestart();

	// Exit 버튼 클릭 — 게임 종료
	UFUNCTION()
	void OnClickExit();

private:
	TWeakObjectPtr<APawn> BoundPawn;            // BindToPlayer 인자 캐시
	TWeakObjectPtr<AVOIDVehicle> BoundVehicle;  // BindToVehicle 인자 캐시
	int32 InstalledCountCached = 0;             // RepairText 갱신용 카운터
};
