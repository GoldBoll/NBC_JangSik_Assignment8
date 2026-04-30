#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VOIDPlayerController.generated.h"

class UUserWidget;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVOIDPlayerController();

	// HUD 위젯 인스턴스 조회 (Vehicle BeginPlay에서 BindToVehicle 호출 시 사용)
	UFUNCTION(BlueprintPure, Category="UI")
	UUserWidget* GetHUDWidgetInstance() const { return HUDWidgetInstance; }

protected:
	// HUD 위젯 생성 + viewport 추가, Pawn 컴포넌트 델리게이트 바인딩
	virtual void BeginPlay() override;
	// Pawn possess 시 HealthComponent OnDeath 바인딩 + HUD BindToPlayer 재시도
	virtual void OnPossess(APawn* InPawn) override;
	// 입력 컴포넌트 셋업 (현재는 부모 기본 동작 사용)
	virtual void SetupInputComponent() override;

	// 플레이어 체력 0 시 호출 — GameMode HandleGameOver 위임
	UFUNCTION()
	void HandlePlayerDeath();

	// BP에서 지정할 HUD 위젯 클래스 (BP_VOIDPlayerController에서 WBP_HUD 할당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// 런타임에 생성된 HUD 위젯 인스턴스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<UUserWidget> HUDWidgetInstance;
};
