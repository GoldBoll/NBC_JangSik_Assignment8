#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VOIDItemInterface.generated.h"

class UVOIDItemDataAsset;

UINTERFACE(MinimalAPI, Blueprintable)
class UVOIDItemInterface : public UInterface
{
	GENERATED_BODY()
};

class VOIDUNREAL_API IVOIDItemInterface
{
	GENERATED_BODY()

public:
	// 픽업 가능한 액터의 ItemData 조회
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Item")
	UVOIDItemDataAsset* GetItemData() const;
	virtual UVOIDItemDataAsset* GetItemData_Implementation() const { return nullptr; }

	// 픽업 시 호출되는 콜백 (인벤토리 추가 후처리용)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Item")
	void OnPickedUp(AActor* Picker);
	virtual void OnPickedUp_Implementation(AActor* Picker) {}
};
