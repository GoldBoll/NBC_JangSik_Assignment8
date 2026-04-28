#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType
#include "VOIDVehicle.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AVOIDVehiclePartSlotActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotInstalled, EVOIDVehiclePartType, PartType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepairComplete);

// Wave 3 탈출 차량 액터
UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDVehicle : public AActor
{
	GENERATED_BODY()

public:
	AVOIDVehicle();

	UFUNCTION(BlueprintCallable, Category="Vehicle")
	void NotifySlotInstalled(EVOIDVehiclePartType PartType);

	UFUNCTION(BlueprintCallable, Category="Vehicle")
	bool TryStartEngine(AActor* Driver);

	UPROPERTY(BlueprintAssignable, Category="Vehicle")
	FOnSlotInstalled OnSlotInstalled;

	UPROPERTY(BlueprintAssignable, Category="Vehicle")
	FOnRepairComplete OnRepairComplete;

protected:
	UPROPERTY(VisibleAnywhere, Category="Vehicle")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, Category="Vehicle")
	TObjectPtr<UBoxComponent> StartEngineVolume;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Vehicle")
	TArray<TObjectPtr<AVOIDVehiclePartSlotActor>> PartSlots;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Vehicle")
	int32 InstalledCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Vehicle")
	bool bRepairComplete = false;
};
