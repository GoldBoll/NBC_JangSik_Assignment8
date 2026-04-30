#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VOIDGameInstance.generated.h"

UCLASS(Blueprintable)
class VOIDUNREAL_API UVOIDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// GameInstance 초기화 — 엔진 시작 시 1회 호출
	virtual void Init() override;

	// 직전 라운드 점수 조회 (게임오버/엔딩 화면 표시용)
	UFUNCTION(BlueprintPure, Category="Stats")
	int32 GetLastRunScore() const { return LastRunScore; }

	// 라운드 종료 시 GameMode가 점수 저장
	UFUNCTION(BlueprintCallable, Category="Stats")
	void SetLastRunScore(int32 NewScore) { LastRunScore = NewScore; }

protected:
	// 레벨 전환에도 살아남는 직전 라운드 점수 (세션 간 전달)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
	int32 LastRunScore = 0;
};
