#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VOIDFloorTransitionTrigger.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDFloorTransitionTrigger : public AActor
{
	GENERATED_BODY()

public:
	AVOIDFloorTransitionTrigger();

	// 진입 시 GameMode StartWave에 전달할 다음 층 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floor", meta=(ClampMin="1", ClampMax="3"))
	int32 TargetFloorIndex = 2;

	// 한 번만 발동 후 비활성화 (역행 방지)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floor")
	bool bConsumeOnTrigger = true;

protected:
	// 플레이어 진입 시 GameMode StartWave 호출
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 트리거 박스 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor")
	TObjectPtr<UBoxComponent> Trigger;

	// 이미 발동되었는지 (bConsumeOnTrigger 가드)
	bool bConsumed = false;
};
