#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType 재사용
#include "VOIDVehiclePartSlot.generated.h"

class UVOIDItemDataAsset;

UINTERFACE(MinimalAPI, Blueprintable)
class UVOIDVehiclePartSlot : public UInterface
{
	GENERATED_BODY()
};

// 차량 부품 슬롯 인터랙션 인터페이스 (Battery / FuelTank / SparkPlug)
class VOIDUNREAL_API IVOIDVehiclePartSlot
{
	GENERATED_BODY()

public:
	// 이 슬롯이 요구하는 부품 종류 (Battery/FuelTank/SparkPlug)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VehicleSlot")
	EVOIDVehiclePartType GetRequiredPartType() const;

	// 부품 설치 시도 — 타입 일치 + 미설치 상태일 때만 성공
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VehicleSlot")
	bool TryInstallPart(UVOIDItemDataAsset* Part, AActor* Installer);

	// 이미 설치 완료된 슬롯인지 (중복 설치 가드용)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VehicleSlot")
	bool IsInstalled() const;
};
