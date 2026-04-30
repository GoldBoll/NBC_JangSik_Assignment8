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

	// Pawn 빙의 시 BT/BB 구동 + AIPerception 바인딩 + 사망 델리게이트 구독
	virtual void OnPossess(APawn* InPawn) override;
	// 매 프레임 — bUseAsyncSight 활성화 시 AsyncTraceInterval마다 RequestAsyncSight
	virtual void Tick(float DeltaSeconds) override;

protected:
	// 좀비 행동 트리 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// 좀비 블랙보드 에셋 (TargetActor / bHasTarget 키)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	// AIPerception 기반 청각 시스템
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	// 청각 자극 감지 설정 (반경/감도/팀 필터)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	// 청각 자극 수신 시 BB TargetActor 갱신
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 사망 처리 1회 가드
	bool bDeathHandled = false;

	// 비동기 Trace 가시선 검증 (AIPerception 청각과 별개)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|AsyncSight")
	bool bUseAsyncSight = false;

	// AsyncLineTrace 호출 주기(초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|AsyncSight", meta=(ClampMin="0.05"))
	float AsyncTraceInterval = 0.2f;

	// 비동기 시야 거리 한계
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|AsyncSight", meta=(ClampMin="100.0"))
	float SightRange = 2500.f;

	// 시야 디버그 라인 표시 (PIE)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|AsyncSight|Debug")
	bool bDrawAsyncSightDebug = true;

	float LastAsyncTraceTime = -1000.f;     // 직전 비동기 Trace 시각
	FTraceDelegate AsyncTraceDelegate;      // Trace 결과 콜백 델리게이트
	bool bAsyncDelegateBound = false;       // 델리게이트 1회 바인딩 가드

	// 비동기 가시선 Trace 요청 (게임 스레드 비차단)
	void RequestAsyncSight();
	// AsyncLineTrace 결과 콜백 — BB TargetActor / bHasTarget 갱신
	void HandleAsyncTrace(const FTraceHandle& Handle, FTraceDatum& Data);
};
