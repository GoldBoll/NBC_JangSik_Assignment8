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

	UE_LOG(LogTemp, Warning, TEXT("[VOID Pickup] %s BeginPlay  bAutoPickup=%d  TriggerSphere=%s  ItemData=%s"),
		*GetName(), bAutoPickup,
		TriggerSphere ? TEXT("OK") : TEXT("NULL"),
		ItemData ? *ItemData->GetName() : TEXT("NULL"));

	if (bAutoPickup && TriggerSphere)
	{
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AVOIDPickupBase::HandleBeginOverlap);
	}
}

void AVOIDPickupBase::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[VOID Pickup] %s overlap by %s"),
		*GetName(), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	if (!OtherActor || !ItemData) { return; }

	UVOIDInventoryComponent* Inv = OtherActor->FindComponentByClass<UVOIDInventoryComponent>();
	if (!Inv)
	{
		UE_LOG(LogTemp, Warning, TEXT("[VOID Pickup]   -> %s has no InventoryComponent"), *OtherActor->GetName());
		return;
	}

	const bool bAdded = Inv->TryAddItem(ItemData, Quantity);
	UE_LOG(LogTemp, Warning, TEXT("[VOID Pickup]   -> TryAddItem returned %d  (Weight now %.2f / %.2f)"),
		bAdded, Inv->GetTotalWeight(), Inv->GetMaxCarry());

	if (bAdded)
	{
		IVOIDItemInterface::Execute_OnPickedUp(this, OtherActor);

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
