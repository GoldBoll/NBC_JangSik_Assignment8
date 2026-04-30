#pragma once

#include "CoreMinimal.h"
#include "Characters/VOIDBaseCharacter.h"
#include "InputActionValue.h"
#include "VOIDPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UVOIDInventoryComponent;
class UVOIDNoiseComponent;
class UVOIDDebuffComponent;
class UVOIDWeaponComponent;
class UInputMappingContext;
class UInputAction;
class UAIPerceptionStimuliSourceComponent;
class UVOIDWeaponConfig;
struct FHitResult;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDPlayerCharacter : public AVOIDBaseCharacter
{
	GENERATED_BODY()

public:
	AVOIDPlayerCharacter();

	// Enhanced Input 액션 5종(Move/Look/Interact/Fire/Aim/Switch) 바인딩
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// 매 프레임 ADS 보간 + 무게비율 동기화
	virtual void Tick(float DeltaTime) override;

protected:
	// 컴포넌트 델리게이트 구독 + IMC 등록
	virtual void BeginPlay() override;

	// 무게 0kg 기준 기본 이동속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float BaseWalkSpeed = 450.0f;

	// MaxCarry 도달 시 이동속도 감소 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0", ClampMax="0.95"))
	float WeightSpeedPenalty = 0.6f;

	// 무게 → 소음 배율 계수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Noise", meta=(ClampMin="0.0"))
	float NoiseWeightFactor = 1.2f;

	// WASD 이동 — Controller Yaw 기준 Forward/Right 입력 분해
	void Move(const FInputActionValue& Value);
	// 마우스 시점 — Controller Yaw/Pitch 회전
	void Look(const FInputActionValue& Value);
	// E 키 인터랙션 — Sweep으로 슬롯/차량/픽업 우선순위 분기
	void Interact(const FInputActionValue& Value);
	// LMB 발사 — WeaponComponent로 위임
	void Fire(const FInputActionValue& Value);

	// 무게 변경 시 이동속도/소음 배율 갱신
	UFUNCTION()
	void OnInventoryWeightChanged(float TotalWeight, float MaxCarry);

	// 체력 감소 감지 — 출혈/골절 디버프 확률 발동
	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealth);

	// 직전 체력값 캐시 (감소량 검출용)
	float LastHealthForBleeding = -1.f;

	// 피격 시 출혈 발동 확률 / 지속 시간
	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float BleedingChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float BleedingDuration = 5.f;

	// 피격 시 골절 발동 확률 / 지속 시간
	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float FractureChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float FractureDuration = 10.f;

	// BP 호환용 Wrapper — 본문은 WeaponComp로 위임
	UFUNCTION(BlueprintCallable, Category="Void|Weapon")
	void EquipWeapon(UVOIDWeaponConfig* NewWeapon);

	// ADS 시작/종료 토글 (RMB 홀드)
	UFUNCTION(BlueprintCallable, Category="Void|ADS")
	void StartAim() { bIsAiming = true; }

	UFUNCTION(BlueprintCallable, Category="Void|ADS")
	void StopAim() { bIsAiming = false; }

	// 매 프레임 ADS FOV/SocketOffset 보간
	void TickAds(float DeltaTime);

	// IA_Aim Started/Completed 시그니처 어댑터
	void OnAimStarted(const FInputActionValue& Value)   { StartAim(); }
	void OnAimCompleted(const FInputActionValue& Value) { StopAim(); }

	// TPS 숄더뷰 카메라 리그 — 오른쪽 어깨 SpringArm + FollowCamera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// 무게 관리 — 인벤토리 슬롯 + MaxCarry 30kg + WeightRatio 산출
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDInventoryComponent> InventoryComponent;

	// 소음 발생 — 발사/픽업/이동 시 좀비 어그로 트리거
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDNoiseComponent> NoiseComponent;

	// 출혈/골절/과적 3종 디버프 관리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDDebuffComponent> DebuffComponent;

	// 무기 시스템 캡슐화 — 발사·반동·장착 담당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDWeaponComponent> WeaponComp;

	// AIPerception 청각 자극원 자동 등록 (좀비가 발사음/발걸음 청각 감지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	// Enhanced Input — 키 매핑 컨텍스트 (BP에서 IMC_Default 할당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// 이동 입력 액션 (Axis2D)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	// 시점 입력 액션 (Mouse XY)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	// 인터랙션 입력 액션 (E)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	// 발사 입력 액션 (LMB)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	// ADS 입력 액션 (RMB 홀드)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	// 라이플 장착 입력 액션 (1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> SwitchToRifleAction;

	// 샷건 장착 입력 액션 (2)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> SwitchToShotgunAction;

	// 라이플 DataAsset (BP Default에 DA_Weapon_Rifle 할당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout")
	TObjectPtr<UVOIDWeaponConfig> RifleConfig;

	// 샷건 DataAsset (BP Default에 DA_Weapon_Shotgun 할당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout")
	TObjectPtr<UVOIDWeaponConfig> ShotgunConfig;

	// 1키 → 라이플 장착
	void OnSwitchToRifle(const FInputActionValue& Value);
	// 2키 → 샷건 장착
	void OnSwitchToShotgun(const FInputActionValue& Value);

	// Hip(평소) FOV — 시야 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float HipFOV = 90.f;

	// ADS(조준) FOV — 줌인 시 시야 좁힘
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float AdsFOV = 55.f;

	// Hip SpringArm 길이 (카메라 거리)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float HipArmLength = 300.f;

	// ADS SpringArm 길이 (조준 시 카메라 가까이)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float AdsArmLength = 180.f;

	// ADS Hip ↔ Aim 보간 속도 (FInterpTo 인자)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float AdsBlendSpeed = 8.f;

	// ADS 시 이동속도 곱셈 (조준 = 이동 느려짐)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS", meta=(ClampMin="0.1", ClampMax="1.0"))
	float AdsMoveMultiplier = 0.6f;

	// 현재 ADS 상태 (RMB 홀드 중 true)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Void|ADS")
	bool bIsAiming = false;
};
