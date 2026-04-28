#include "Vehicle/VOIDVehicle.h"

#include "Vehicle/VOIDVehiclePartSlotActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

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

	// TODO: AVOIDGameMode::HandleEscapeSuccess(Driver) 본 구현
	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
	{
		(void)GM;
		UE_LOG(LogTemp, Display, TEXT("[Vehicle] Engine started by %s"), *GetNameSafe(Driver));
	}
	return true;
}
