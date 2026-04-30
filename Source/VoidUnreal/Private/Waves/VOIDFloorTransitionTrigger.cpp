#include "Waves/VOIDFloorTransitionTrigger.h"
#include "Components/BoxComponent.h"
#include "Core/VOIDGameMode.h"
#include "Characters/VOIDPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Trigger 볼륨 생성 (150x150x200) + BeginOverlap 바인딩
AVOIDFloorTransitionTrigger::AVOIDFloorTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(150.0f, 150.0f, 200.0f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AVOIDFloorTransitionTrigger::HandleBeginOverlap);
	RootComponent = Trigger;
}

// 플레이어 진입 + 현재 웨이브 번호 == TargetFloorIndex - 1 일 때 AdvanceWave
void AVOIDFloorTransitionTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bConsumed || !OtherActor) { return; }
	if (!OtherActor->IsA(AVOIDPlayerCharacter::StaticClass())) { return; }

	AVOIDGameMode* GM = Cast<AVOIDGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM) { return; }

	if (GM->GetCurrentWaveIndex() + 1 != TargetFloorIndex) { return; }

	GM->AdvanceWave();

	if (bConsumeOnTrigger)
	{
		bConsumed = true;
		Trigger->SetGenerateOverlapEvents(false);
	}
}
