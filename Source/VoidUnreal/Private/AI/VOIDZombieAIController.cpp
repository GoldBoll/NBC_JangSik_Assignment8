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
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AVOIDZombieAIController::AVOIDZombieAIController()
{
	// 사망 폴링용
	PrimaryActorTick.bCanEverTick = true;

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3000.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies   = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals  = true;

	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->SetDominantSense(HearingConfig->GetSenseImplementation());

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

	// 비동기 Trace 가시선 검증 (bUseAsyncSight 옵션)
	if (bUseAsyncSight && !bDeathHandled)
	{
		if (!bAsyncDelegateBound)
		{
			AsyncTraceDelegate.BindUObject(this, &AVOIDZombieAIController::HandleAsyncTrace);
			bAsyncDelegateBound = true;
		}

		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		if (Now - LastAsyncTraceTime >= AsyncTraceInterval)
		{
			LastAsyncTraceTime = Now;
			RequestAsyncSight();
		}
	}

	if (bDeathHandled) return;

	auto* Zombie = Cast<AVOIDZombieCharacter>(GetPawn());
	if (!Zombie || !Zombie->IsDead()) return;

	bDeathHandled = true;

	if (UBrainComponent* Brain = GetBrainComponent())
		Brain->StopLogic(TEXT("Dead"));

	if (auto* Movement = Zombie->GetCharacterMovement())
		Movement->DisableMovement();

	if (auto* Capsule = Zombie->GetCapsuleComponent())
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (auto* Mesh = Zombie->GetMesh())
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 5초 후 자동 제거 (시체 + 컨트롤러)
	Zombie->SetLifeSpan(5.0f);
	SetLifeSpan(5.0f);

	UE_LOG(LogTemp, Warning, TEXT("Zombie %s died — BT stopped, lifespan 5s"),
	       *Zombie->GetName());
}

void AVOIDZombieAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Stimulus.WasSuccessfullySensed()) return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsVector(TEXT("LastNoiseLocation"), Stimulus.StimulusLocation);

	if (Actor && Actor->IsA(APawn::StaticClass()) && Actor != GetPawn())
	{
		BB->SetValueAsObject(TEXT("TargetActor"), Actor);
		BB->SetValueAsBool(TEXT("bHasTarget"), true);
	}
}

void AVOIDZombieAIController::RequestAsyncSight()
{
	APawn* Self = GetPawn();
	AActor* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!IsValid(Self) || !IsValid(Player) || !GetWorld()) return;

	const FVector Start = Self->GetActorLocation()  + FVector(0, 0, 60);
	const FVector End   = Player->GetActorLocation() + FVector(0, 0, 60);

	if ((End - Start).SizeSquared() > SightRange * SightRange) return;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(VOIDAsyncSight), false, Self);
	Params.AddIgnoredActor(Player);

	GetWorld()->AsyncLineTraceByChannel(
		EAsyncTraceType::Single,
		Start, End,
		ECC_Visibility,
		Params,
		FCollisionResponseParams::DefaultResponseParam,
		&AsyncTraceDelegate);
}

void AVOIDZombieAIController::HandleAsyncTrace(const FTraceHandle& Handle, FTraceDatum& Data)
{
	if (!IsValid(this) || !IsValid(GetPawn())) return;

	// bBlocking==false → 가시선 확보 → 타깃 갱신
	const bool bBlocked = (Data.OutHits.Num() > 0 && Data.OutHits[0].bBlockingHit);

#if !(UE_BUILD_SHIPPING)
	if (bDrawAsyncSightDebug)
	{
		if (UWorld* World = GetWorld())
		{
			const FVector DrawEnd = bBlocked ? FVector(Data.OutHits[0].ImpactPoint) : Data.End;
			const FColor  LineColor = bBlocked ? FColor::Red : FColor::Green;
			const float   Lifetime  = AsyncTraceInterval * 1.1f;

			DrawDebugLine(World, Data.Start, DrawEnd, LineColor, false, Lifetime, 0, 2.f);
			if (bBlocked)
			{
				DrawDebugSphere(World, DrawEnd, 12.f, 8, FColor::Yellow, false, Lifetime);
			}
			else
			{
				DrawDebugSphere(World, Data.End, 18.f, 12, FColor::Cyan, false, Lifetime);
			}
		}
	}
#endif

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		if (!bBlocked)
		{
			BB->SetValueAsObject(TEXT("TargetActor"), UGameplayStatics::GetPlayerPawn(this, 0));
			BB->SetValueAsBool(TEXT("bHasTarget"), true);
		}
	}
}
