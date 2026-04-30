#include "Vehicle/VOIDVehiclePartSlotActor.h"

#include "Vehicle/VOIDVehicle.h"
#include "Items/VOIDItemDataAsset.h"
#include "Components/VOIDInventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// InteractionVolume(반경 180, Visibility=Block) + EmptyMesh/InstalledMesh 생성
AVOIDVehiclePartSlotActor::AVOIDVehiclePartSlotActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	SetRootComponent(InteractionVolume);
	InteractionVolume->InitSphereRadius(180.f);
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
	InteractionVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionVolume->SetGenerateOverlapEvents(true);

	EmptyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EmptyMesh"));
	EmptyMesh->SetupAttachment(InteractionVolume);
	EmptyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InstalledMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InstalledMesh"));
	InstalledMesh->SetupAttachment(InteractionVolume);
	InstalledMesh->SetVisibility(false);
	InstalledMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 타입 검증 → 인벤 차감 → 메시 교체 → OwnerVehicle 슬롯 통보
bool AVOIDVehiclePartSlotActor::TryInstallPart_Implementation(UVOIDItemDataAsset* Part, AActor* Installer)
{
	if (bInstalled || !IsValid(Part) || !IsValid(Installer)) return false;

	if (Part->Category != EVOIDItemCategory::VehiclePart || Part->PartType != RequiredType)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Slot] %s 부품 타입 불일치 — 필요=%s 후보=%s"),
			*GetName(),
			*UEnum::GetValueAsString(RequiredType),
			*UEnum::GetValueAsString(Part->PartType));
		return false;
	}

	UVOIDInventoryComponent* Inv = Installer->FindComponentByClass<UVOIDInventoryComponent>();
	if (!IsValid(Inv) || !Inv->RemoveItem(Part, 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Slot] %s RemoveItem 실패"), *GetName());
		return false;
	}

	bInstalled = true;
	if (EmptyMesh)     EmptyMesh->SetVisibility(false);
	if (InstalledMesh) InstalledMesh->SetVisibility(true);

	UE_LOG(LogTemp, Warning, TEXT("[Slot] %s 부품 설치 완료 (%s) — Weight now %.2f / %.2f"),
		*GetName(),
		*UEnum::GetValueAsString(RequiredType),
		Inv->GetTotalWeight(), Inv->GetMaxCarry());

	// OwnerVehicle 이 비어있으면 ParentActor (ChildActorComponent 부모) 에서 자동 검색
	if (!OwnerVehicle.IsValid())
	{
		for (AActor* Cur = GetParentActor(); Cur; Cur = Cur->GetParentActor())
		{
			if (auto* V = Cast<AVOIDVehicle>(Cur)) { OwnerVehicle = V; break; }
		}
	}

	if (OwnerVehicle.IsValid())
	{
		OwnerVehicle->NotifySlotInstalled(RequiredType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Slot] %s OwnerVehicle 미설정 — NotifySlotInstalled 호출 안 됨"), *GetName());
	}
	return true;
}
