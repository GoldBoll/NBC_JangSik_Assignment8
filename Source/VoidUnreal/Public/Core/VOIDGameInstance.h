#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VOIDGameInstance.generated.h"

UCLASS(Blueprintable)
class VOIDUNREAL_API UVOIDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintPure, Category="Stats")
	int32 GetLastRunScore() const { return LastRunScore; }

	UFUNCTION(BlueprintCallable, Category="Stats")
	void SetLastRunScore(int32 NewScore) { LastRunScore = NewScore; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
	int32 LastRunScore = 0;
};
