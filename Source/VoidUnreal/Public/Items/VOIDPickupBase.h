#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/VOIDItemInterface.h"
#include "VOIDPickupBase.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UVOIDItemDataAsset;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDPickupBase : public AActor, public IVOIDItemInterface
{
	GENERATED_BODY()

public:
	AVOIDPickupBase();

	virtual UVOIDItemDataAsset* GetItemData_Implementation() const override { return ItemData; }
	virtual void OnPickedUp_Implementation(AActor* Picker) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UVOIDItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	bool bAutoPickup = true;
};
