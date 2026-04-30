#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VOIDWeaponConfig.generated.h"

class USoundBase;
class UCameraShakeBase;
class UTexture2D;

// 무기 분류 — DA 인스턴스가 어떤 종류인지 표시
UENUM(BlueprintType)
enum class EVOIDWeaponClass : uint8
{
	Rifle    UMETA(DisplayName="Rifle"),
	Shotgun  UMETA(DisplayName="Shotgun"),
	Pistol   UMETA(DisplayName="Pistol")
};

// 무기 발사·반동·소음 파라미터 DataAsset
UCLASS(BlueprintType)
class VOIDUNREAL_API UVOIDWeaponConfig : public UDataAsset
{
	GENERATED_BODY()

public:
    // 무기의 기본 분류 (예: 소총, 권총 등).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
    EVOIDWeaponClass WeaponClass = EVOIDWeaponClass::Rifle;

    // 해당 무기 데이터의 고유 식별자.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
    FName WeaponId = NAME_None;

    // HUD 표기용 무기 이름 (없으면 WeaponId 사용).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
    FText DisplayName;

    // HUD 좌측 무기 아이콘 (선택).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
    TObjectPtr<UTexture2D> Icon;

    // 한 번 발사할 때 나가는 투사체(탄알)의 개수 (샷건 등에 사용).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire", meta=(ClampMin="1"))
    int32 PelletCount = 1;

    // 탄알이 퍼지는 최대 각도 (명중률).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire", meta=(ClampMin="0.0"))
    float SpreadDegrees = 0.f;

    // 투사체의 최대 유효 사거리.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire", meta=(ClampMin="0.0"))
    float Range = 5000.f;

    // 탄알 한 발당 가해지는 기본 데미지.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire", meta=(ClampMin="0.0"))
    float DamagePerPellet = 12.f;

    // 다음 사격까지의 시간 간격 (연사 속도).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Fire", meta=(ClampMin="0.05"))
    float FireInterval = 0.12f;

    // 발사 시 수직으로 튀는 반동의 세기.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Recoil", meta=(ClampMin="0.0"))
    float RecoilPitch = 1.5f;

    // 발사 시 좌우로 불규칙하게 흔들리는 반동 범위.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Recoil", meta=(ClampMin="0.0"))
    float RecoilYawJitter = 0.6f;

    // 초당 반동이 원래 위치로 돌아오는 회복 속도.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Recoil", meta=(ClampMin="0.1"))
    float RecoilRecoverPerSec = 4.f;

    // 발사 시 AI 등이 감지할 수 있는 소음 반경.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Noise", meta=(ClampMin="0.0"))
    float NoiseRadius = 4500.f;

    // 발사 시 재생할 사운드 에셋 (지연 로딩 방식).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|FX")
    TSoftObjectPtr<USoundBase> FireSound;

    // 발사 시 플레이어 카메라에 적용할 흔들림 효과 (지연 로딩 방식).
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|FX")
    TSoftClassPtr<UCameraShakeBase> CameraShakeClass;
};
