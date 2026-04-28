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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VehicleSlot")
	EVOIDVehiclePartType GetRequiredPartType() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VehicleSlot")
	bool TryInstallPart(UVOIDItemDataAsset* Part, AActor* Installer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="VehicleSlot")
	bool IsInstalled() const;
};
