#include "Items/VOIDPickupBase.h"
#include "Items/VOIDItemDataAsset.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/VOIDInventoryComponent.h"
#include "Components/VOIDNoiseComponent.h"

AVOIDPickupBase::AVOIDPickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(80.0f);
	TriggerSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = TriggerSphere;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVOIDPickupBase::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoPickup && TriggerSphere)
	{
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AVOIDPickupBase::HandleBeginOverlap);
	}
}

void AVOIDPickupBase::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !ItemData) { return; }

	UVOIDInventoryComponent* Inv = OtherActor->FindComponentByClass<UVOIDInventoryComponent>();
	if (!Inv) { return; }

	if (Inv->TryAddItem(ItemData, Quantity))
	{
		IVOIDItemInterface::Execute_OnPickedUp(this, OtherActor);

		// 픽업 소음 방출 (Void 게임 콘셉트: Noise is Currency)
		if (UVOIDNoiseComponent* Noise = OtherActor->FindComponentByClass<UVOIDNoiseComponent>())
		{
			Noise->EmitNoise(EVOIDNoiseSource::Pickup, 1.0f);
		}

		Destroy();
	}
}

void AVOIDPickupBase::OnPickedUp_Implementation(AActor* Picker)
{
	// 블루프린트에서 VFX/SFX 확장
}
