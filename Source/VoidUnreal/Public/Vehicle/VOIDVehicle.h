#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/VOIDVehiclePart.h"  // EVOIDVehiclePartType
#include "VOIDVehicle.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AVOIDVehiclePartSlotActor;

// 슬롯 설치 / 수리 완료 시 HUD가 구독하는 멀티캐스트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotInstalled, EVOIDVehiclePartType, PartType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepairComplete);

// Wave 3 탈출 차량 액터
UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDVehicle : public AActor
{
	GENERATED_BODY()

public:
	AVOIDVehicle();

protected:
	virtual void BeginPlay() override;

public:
	// 슬롯에서 부품 설치 완료 시 호출 (InstalledCount 증가 + 3 도달 시 RepairComplete)
	UFUNCTION(BlueprintCallable, Category="Vehicle")
	void NotifySlotInstalled(EVOIDVehiclePartType PartType);

	// 시동 시도 — 수리 완료 시 GameMode HandleEscapeSuccess 호출
	UFUNCTION(BlueprintCallable, Category="Vehicle")
	bool TryStartEngine(AActor* Driver);

	// 슬롯 설치 이벤트 — HUD RepairText 구독
	UPROPERTY(BlueprintAssignable, Category="Vehicle")
	FOnSlotInstalled OnSlotInstalled;

	// 수리 완료 이벤트 — HUD가 "Press E to Start" 표시
	UPROPERTY(BlueprintAssignable, Category="Vehicle")
	FOnRepairComplete OnRepairComplete;

protected:
	// 차량 본체 메시 (Sweep 인터랙션 대상)
	UPROPERTY(VisibleAnywhere, Category="Vehicle")
	TObjectPtr<UStaticMeshComponent> Body;

	// 시동 인터랙션 트리거 박스 (E 키 진입점)
	UPROPERTY(VisibleAnywhere, Category="Vehicle")
	TObjectPtr<UBoxComponent> StartEngineVolume;

	// 자식 슬롯 액터 3개 참조 (Battery/FuelTank/SparkPlug)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Vehicle")
	TArray<TObjectPtr<AVOIDVehiclePartSlotActor>> PartSlots;

	// 설치된 부품 수 (0~3, 3 도달 시 수리 완료)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Vehicle")
	int32 InstalledCount = 0;

	// 모든 슬롯 설치 완료 여부 (시동 가드)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Vehicle")
	bool bRepairComplete = false;
};
