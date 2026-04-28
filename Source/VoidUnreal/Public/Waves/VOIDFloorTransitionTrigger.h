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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floor", meta=(ClampMin="1", ClampMax="3"))
	int32 TargetFloorIndex = 2;

	// 한 번만 발동 후 비활성화 (역행 방지)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floor")
	bool bConsumeOnTrigger = true;

protected:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Floor")
	TObjectPtr<UBoxComponent> Trigger;

	bool bConsumed = false;
};
