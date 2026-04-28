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

	// IVOIDVehiclePartSlot
	virtual EVOIDVehiclePartType GetRequiredPartType_Implementation() const override { return RequiredType; }
	virtual bool TryInstallPart_Implementation(UVOIDItemDataAsset* Part, AActor* Installer) override;
	virtual bool IsInstalled_Implementation() const override { return bInstalled; }

protected:
	UPROPERTY(VisibleAnywhere, Category="VehicleSlot")
	TObjectPtr<USphereComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, Category="VehicleSlot")
	TObjectPtr<UStaticMeshComponent> EmptyMesh;

	UPROPERTY(VisibleAnywhere, Category="VehicleSlot")
	TObjectPtr<UStaticMeshComponent> InstalledMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VehicleSlot")
	EVOIDVehiclePartType RequiredType = EVOIDVehiclePartType::Battery;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="VehicleSlot")
	bool bInstalled = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="VehicleSlot")
	TWeakObjectPtr<AVOIDVehicle> OwnerVehicle;
};
