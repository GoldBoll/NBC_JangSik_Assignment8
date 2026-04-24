#pragma once

#include "CoreMinimal.h"
#include "Items/VOIDPickupBase.h"
#include "VOIDVehiclePart.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VehiclePart")
	EVOIDVehiclePartType PartType = EVOIDVehiclePartType::None;
};
