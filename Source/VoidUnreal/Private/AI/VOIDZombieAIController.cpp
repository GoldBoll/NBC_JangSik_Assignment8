#include "AI/VOIDZombieAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/VOIDZombieCharacter.h"

AVOIDZombieAIController::AVOIDZombieAIController()
{
	// 죽음 폴링용 — 좀비 사망 시 BT/이동 정지하기 위해
	PrimaryActorTick.bCanEverTick = true;

	// AIPerception 컴포넌트 생성
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

	// Hearing Sense 설정
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3000.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies   = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals  = true;

	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->SetDominantSense(HearingConfig->GetSenseImplementation());

	// 인지 이벤트 콜백 바인딩
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
		this, &AVOIDZombieAIController::OnPerceptionUpdated);
}

void AVOIDZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BlackboardAsset)
	{
		UBlackboardComponent* BlackboardComponent = nullptr;
		UseBlackboard(BlackboardAsset, BlackboardComponent);
	}

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AVOIDZombieAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 이미 죽음 처리했으면 종료 (SetLifeSpan 타이머 리셋 방지)
	if (bDeathHandled) return;

	auto* Zombie = Cast<AVOIDZombieCharacter>(GetPawn());
	if (!Zombie || !Zombie->IsDead()) return;

	// 처리 시작 — 플래그를 즉시 올려 다음 프레임 진입 차단
	bDeathHandled = true;

	// 1. BT 정지 — MoveTo·BTTask_ZombieAttack 즉시 중단
	if (UBrainComponent* Brain = GetBrainComponent())
		Brain->StopLogic(TEXT("Dead"));

	// 2. 이동 중지 — 죽은 후 미끄러짐 방지
	if (auto* Movement = Zombie->GetCharacterMovement())
		Movement->DisableMovement();

	// 3. 캡슐 + 메시 콜리전 끄기 — 시체에 사격·이동 안 막힘
	if (auto* Capsule = Zombie->GetCapsuleComponent())
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (auto* Mesh = Zombie->GetMesh())
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 4. 5초 후 자동 제거 (시체 정리) — 좀비와 컨트롤러 양쪽에 적용
	Zombie->SetLifeSpan(5.0f);
	SetLifeSpan(5.0f);  // AIController도 함께 정리

	UE_LOG(LogTemp, Warning, TEXT("Zombie %s died — BT stopped, lifespan 5s"),
	       *Zombie->GetName());
}

void AVOIDZombieAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지 실패(소음 사라짐) 이벤트는 무시
	if (!Stimulus.WasSuccessfullySensed()) return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	// 1. 청각 자극 위치를 BB에 저장 → BT의 Chase 분기 트리거
	BB->SetValueAsVector(TEXT("LastNoiseLocation"), Stimulus.StimulusLocation);

	// 2. 소음 발신자가 Pawn(플레이어)이고 자기 자신이 아니면 공격 대상 지정
	//    → BT의 Attack 분기 트리거 (bHasTarget Is Set 데코레이터 통과)
	if (Actor && Actor->IsA(APawn::StaticClass()) && Actor != GetPawn())
	{
		BB->SetValueAsObject(TEXT("TargetActor"), Actor);
		BB->SetValueAsBool(TEXT("bHasTarget"), true);
	}

	UE_LOG(LogTemp, Log, TEXT("Zombie %s perceived noise from %s at %s"),
	       GetPawn() ? *GetPawn()->GetName() : TEXT("Unknown"),
	       Actor ? *Actor->GetName() : TEXT("Unknown"),
	       *Stimulus.StimulusLocation.ToString());
}
