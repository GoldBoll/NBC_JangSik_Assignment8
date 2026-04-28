#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WorldCollision.h"  // FTraceDelegate / FTraceHandle / FTraceDatum
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

	// 사망 처리 1회 가드
	bool bDeathHandled = false;

	// 비동기 Trace 가시선 검증 (AIPerception 청각과 별개)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|AsyncSight")
	bool bUseAsyncSight = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|AsyncSight", meta=(ClampMin="0.05"))
	float AsyncTraceInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|AsyncSight", meta=(ClampMin="100.0"))
	float SightRange = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|AsyncSight|Debug")
	bool bDrawAsyncSightDebug = true;

	float LastAsyncTraceTime = -1000.f;
	FTraceDelegate AsyncTraceDelegate;
	bool bAsyncDelegateBound = false;

	void RequestAsyncSight();
	void HandleAsyncTrace(const FTraceHandle& Handle, FTraceDatum& Data);
};
