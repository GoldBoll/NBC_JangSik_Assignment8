#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VOIDSpawnVolume.generated.h"

class UBoxComponent;

// 스폰 볼륨이 처리할 액터 종류
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

	// 볼륨 박스 내부 임의 좌표 반환 + 라인트레이스 바닥 안착
	UFUNCTION(BlueprintCallable, Category="Spawn")
	FVector GetRandomPointInVolume() const;

	// 지정 클래스 액터 1개 생성 (좀비/픽업)
	UFUNCTION(BlueprintCallable, Category="Spawn")
	AActor* SpawnActorOfClass(TSubclassOf<AActor> ActorClass);

	// 이 볼륨이 처리할 종류 (좀비/아이템/둘 다)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	EVOIDSpawnKind SpawnKind = EVOIDSpawnKind::Zombie;

	// 이 볼륨이 속한 층 (1=로비, 2=거주, 3=옥상)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn", meta=(ClampMin="1", ClampMax="3"))
	int32 FloorIndex = 1;

protected:
	// 스폰 영역 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn")
	TObjectPtr<UBoxComponent> Bounds;
};
