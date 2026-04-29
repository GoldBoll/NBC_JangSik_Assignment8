#include "Vehicle/VOIDVehicle.h"

#include "Vehicle/VOIDVehiclePartSlotActor.h"
#include "Core/VOIDGameMode.h"
#include "Core/VOIDPlayerController.h"
#include "UI/VOIDHUDWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"

AVOIDVehicle::AVOIDVehicle()
{
	PrimaryActorTick.bCanEverTick = false;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	SetRootComponent(Body);

	StartEngineVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("StartEngineVolume"));
	StartEngineVolume->SetupAttachment(Body);
	StartEngineVolume->SetBoxExtent(FVector(120.f, 120.f, 80.f));
	StartEngineVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AVOIDVehicle::BeginPlay()
{
	Super::BeginPlay();

	// PC BeginPlay 보다 먼저 실행될 수 있어 1 tick 지연으로 HUD 바인딩
	GetWorldTimerManager().SetTimerForNextTick([WeakThis = TWeakObjectPtr<AVOIDVehicle>(this)]()
	{
		if (!WeakThis.IsValid()) { return; }
		AVOIDVehicle* Self = WeakThis.Get();
		APlayerController* PC = UGameplayStatics::GetPlayerController(Self, 0);
		auto* VPC = Cast<AVOIDPlayerController>(PC);
		if (!VPC) { return; }
		auto* HUD = Cast<UVOIDHUDWidget>(VPC->GetHUDWidgetInstance());
		if (!HUD) { return; }
		HUD->BindToVehicle(Self);
	});
}

void AVOIDVehicle::NotifySlotInstalled(EVOIDVehiclePartType PartType)
{
	++InstalledCount;
	OnSlotInstalled.Broadcast(PartType);

	if (!bRepairComplete && InstalledCount >= 3)
	{
		bRepairComplete = true;
		OnRepairComplete.Broadcast();
	}
}

bool AVOIDVehicle::TryStartEngine(AActor* Driver)
{
	if (!bRepairComplete) return false;

	UE_LOG(LogTemp, Display, TEXT("[Vehicle] Engine started by %s"), *GetNameSafe(Driver));

	if (auto* GM = Cast<AVOIDGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->HandleEscapeSuccess(Driver);
	}
	return true;
}
