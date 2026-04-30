#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/VOIDVehiclePartSlot.h"
#include "Items/VOIDVehiclePart.h"
#include "VOIDVehiclePartSlotActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AVOIDVehicle;
class UVOIDItemDataAsset;

// 차량 부품 1개 슬롯 — 빈/장착 메시 토글 + 인벤토리 차감
UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDVehiclePartSlotActor : public AActor, public IVOIDVehiclePartSlot
{
	GENERATED_BODY()

public:
	AVOIDVehiclePartSlotActor();

	// IVOIDVehiclePartSlot 구현 — 슬롯의 요구 부품 종류 반환
	virtual EVOIDVehiclePartType GetRequiredPartType_Implementation() const override { return RequiredType; }
	// 부품 설치 시도 — 타입 일치 + 인벤토리 차감 + OwnerVehicle 통보
	virtual bool TryInstallPart_Implementation(UVOIDItemDataAsset* Part, AActor* Installer) override;
	// 설치 완료 여부
	virtual bool IsInstalled_Implementation() const override { return bInstalled; }

protected:
	// E 키 인터랙션 Sweep이 잡을 콜리전 영역
	UPROPERTY(VisibleAnywhere, Category="VehicleSlot")
	TObjectPtr<USphereComponent> InteractionVolume;

	// 빈 슬롯 메시 (부품 미설치 시 표시)
	UPROPERTY(VisibleAnywhere, Category="VehicleSlot")
	TObjectPtr<UStaticMeshComponent> EmptyMesh;

	// 장착 메시 (부품 설치 후 표시)
	UPROPERTY(VisibleAnywhere, Category="VehicleSlot")
	TObjectPtr<UStaticMeshComponent> InstalledMesh;

	// 이 슬롯이 받을 부품 종류 (BP 인스턴스에서 Battery/FuelTank/SparkPlug 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VehicleSlot")
	EVOIDVehiclePartType RequiredType = EVOIDVehiclePartType::Battery;

	// 이미 설치 완료된 슬롯인지
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="VehicleSlot")
	bool bInstalled = false;

	// 소속 차량 약참조 — NotifySlotInstalled 호출용 (자동 fallback 검색 있음)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="VehicleSlot")
	TWeakObjectPtr<AVOIDVehicle> OwnerVehicle;
};
