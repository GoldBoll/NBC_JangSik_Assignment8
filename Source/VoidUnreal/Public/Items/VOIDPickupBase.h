#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/VOIDItemInterface.h"
#include "VOIDPickupBase.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UVOIDItemDataAsset;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDPickupBase : public AActor, public IVOIDItemInterface
{
	GENERATED_BODY()

public:
	AVOIDPickupBase();

	// 인터페이스 구현 — 보유 ItemData 반환
	virtual UVOIDItemDataAsset* GetItemData_Implementation() const override { return ItemData; }
	// 인터페이스 구현 — 인벤토리 추가 + 소음(Pickup) 발생 후 자기 파괴
	virtual void OnPickedUp_Implementation(AActor* Picker) override;

protected:
	virtual void BeginPlay() override;

	// 트리거 Sphere에 캐릭터 진입 시 — 자동 픽업이면 OnPickedUp 호출
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 자동 픽업 트리거 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USphereComponent> TriggerSphere;

	// 시각 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	// 픽업 시 인벤토리에 추가할 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UVOIDItemDataAsset> ItemData;

	// 한 번 픽업 시 추가될 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	int32 Quantity = 1;

	// true=Sphere Overlap만으로 자동 픽업 / false=E 키로 명시 픽업
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	bool bAutoPickup = true;
};
