#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VOIDWeaponComponent.generated.h"

class UVOIDWeaponConfig;

// 무기 장착 이벤트 (해제 시 NewWeapon=nullptr)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, UVOIDWeaponConfig*, NewWeapon);

// 무기 발사 이벤트 (마지막 펠릿 임팩트 좌표 + 마지막 적중 액터)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponFired, FVector, ImpactPoint, AActor*, HitActor);

// 무기 시스템 캡슐화 — WeaponConfig 보관 + 발사 + 반동 누적/회복
UCLASS(Blueprintable, ClassGroup=(VOID), meta=(BlueprintSpawnableComponent))
class VOIDUNREAL_API UVOIDWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVOIDWeaponComponent();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void EquipWeapon(UVOIDWeaponConfig* NewWeapon);

	// 발사 시도. FireInterval 컷 시 false. 산탄/데미지/사운드/반동까지 일괄 처리.
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool TryFire(const FVector& MuzzleLocation, const FRotator& AimRotation,
	             AActor* DamageInstigator, float SpreadMultiplier = 1.0f);

	// 반동 누적값 회복 + 이번 프레임 입력 델타 반환 (Pawn이 컨트롤러에 적용)
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void TickRecoil(float DeltaTime, float& OutPitchDelta, float& OutYawDelta);

	UFUNCTION(BlueprintPure, Category="Weapon")
	UVOIDWeaponConfig* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category="Weapon")
	bool CanFireNow() const;

	UPROPERTY(BlueprintAssignable, Category="Weapon")
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY(BlueprintAssignable, Category="Weapon")
	FOnWeaponFired OnWeaponFired;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UVOIDWeaponConfig> CurrentWeapon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Weapon")
	float LastFireTime = -1000.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Weapon")
	float PendingRecoilPitch = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Weapon")
	float PendingRecoilYaw = 0.f;

	// 스프링-댐프 진동 속도 (damped sine recover에 사용)
	float RecoilVelocityPitch = 0.f;
	float RecoilVelocityYaw   = 0.f;

	// 직전 프레임에 적용된 누적 표시값 — 변화량 계산용
	float PrevAppliedPitch = 0.f;
	float PrevAppliedYaw   = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Debug")
	bool bDrawWeaponDebug = true;

private:
	// 단일 펠릿 트레이스 + 데미지 (TryFire 산탄 루프에서 호출)
	void FireSinglePellet(const FVector& Start, const FVector& Dir, float Range, float Damage,
	                      AActor* DamageInstigator, FHitResult& OutHit, bool& bOutHit);

	void ApplyHitDamage(const FHitResult& Hit, float Damage, AActor* DamageInstigator);
};
