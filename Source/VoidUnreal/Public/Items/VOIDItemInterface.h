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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Item")
	UVOIDItemDataAsset* GetItemData() const;
	virtual UVOIDItemDataAsset* GetItemData_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Item")
	void OnPickedUp(AActor* Picker);
	virtual void OnPickedUp_Implementation(AActor* Picker) {}
};
