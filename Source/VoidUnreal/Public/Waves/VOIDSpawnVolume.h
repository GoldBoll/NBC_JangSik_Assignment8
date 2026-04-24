#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VOIDSpawnVolume.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EVOIDSpawnKind : uint8
{
	Zombie   UMETA(DisplayName="좀비"),
	Item     UMETA(DisplayName="아이템"),
	Both     UMETA(DisplayName="좀비 + 아이템")
};

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDSpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	AVOIDSpawnVolume();

	UFUNCTION(BlueprintCallable, Category="Spawn")
	FVector GetRandomPointInVolume() const;

	UFUNCTION(BlueprintCallable, Category="Spawn")
	AActor* SpawnActorOfClass(TSubclassOf<AActor> ActorClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	EVOIDSpawnKind SpawnKind = EVOIDSpawnKind::Zombie;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn")
	TObjectPtr<UBoxComponent> Bounds;
};
