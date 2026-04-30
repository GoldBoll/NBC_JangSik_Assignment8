#pragma once

#include "CoreMinimal.h"
#include "Items/VOIDPickupBase.h"
#include "VOIDVehiclePart.generated.h"

// 차량 부품 종류 — 슬롯 매칭 키
UENUM(BlueprintType)
enum class EVOIDVehiclePartType : uint8
{
	None        UMETA(Hidden),
	Battery     UMETA(DisplayName="배터리"),      // 8kg
	FuelTank    UMETA(DisplayName="연료통"),      // 12kg
	SparkPlug   UMETA(DisplayName="점화 플러그")  // 2kg
};

// Wave 3 탈출 조건용 차량 부품. 무게가 커서 '이동 속도 ↓·소음 ↑'를 유발한다.
UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDVehiclePart : public AVOIDPickupBase
{
	GENERATED_BODY()

public:
	// 이 부품의 종류 (슬롯의 RequiredType과 일치해야 설치 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VehiclePart")
	EVOIDVehiclePartType PartType = EVOIDVehiclePartType::None;
};
