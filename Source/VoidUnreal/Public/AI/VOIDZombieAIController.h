#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "VOIDZombieAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
struct FAIStimulus;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	AVOIDZombieAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	// AIPerception 기반 청각 시스템
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 죽음 처리 1회 보장 플래그 — Tick에서 IsDead 폴링 시 중복 SetLifeSpan 방지
	bool bDeathHandled = false;
};
