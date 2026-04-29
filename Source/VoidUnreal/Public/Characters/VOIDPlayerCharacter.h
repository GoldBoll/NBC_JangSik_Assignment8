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

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

protected:
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

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);

	UFUNCTION()
	void OnInventoryWeightChanged(float TotalWeight, float MaxCarry);

	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealth);

	float LastHealthForBleeding = -1.f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float BleedingChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float BleedingDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float FractureChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float FractureDuration = 10.f;

	// BP 호환용 Wrapper — 본문은 WeaponComp로 위임
	UFUNCTION(BlueprintCallable, Category="Void|Weapon")
	void EquipWeapon(UVOIDWeaponConfig* NewWeapon);

	UFUNCTION(BlueprintCallable, Category="Void|ADS")
	void StartAim() { bIsAiming = true; }

	UFUNCTION(BlueprintCallable, Category="Void|ADS")
	void StopAim() { bIsAiming = false; }

	void TickAds(float DeltaTime);

	// IA_Aim Started/Completed 시그니처 어댑터
	void OnAimStarted(const FInputActionValue& Value)   { StartAim(); }
	void OnAimCompleted(const FInputActionValue& Value) { StopAim(); }

	// TPS 숄더뷰 카메라 리그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// Void 핵심 컴포넌트 (무게·소음·디버프)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDNoiseComponent> NoiseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDDebuffComponent> DebuffComponent;

	// 무기 시스템 캡슐화 — 발사·반동·장착 담당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDWeaponComponent> WeaponComp;

	// AIPerception 청각 자극원 자동 등록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	// Enhanced Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> SwitchToRifleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> SwitchToShotgunAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout")
	TObjectPtr<UVOIDWeaponConfig> RifleConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Loadout")
	TObjectPtr<UVOIDWeaponConfig> ShotgunConfig;

	void OnSwitchToRifle(const FInputActionValue& Value);
	void OnSwitchToShotgun(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float HipFOV = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float AdsFOV = 55.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float HipArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float AdsArmLength = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS")
	float AdsBlendSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Void|ADS", meta=(ClampMin="0.1", ClampMax="1.0"))
	float AdsMoveMultiplier = 0.6f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Void|ADS")
	bool bIsAiming = false;
};
