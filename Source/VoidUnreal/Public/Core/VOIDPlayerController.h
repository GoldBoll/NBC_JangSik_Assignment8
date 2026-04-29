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

	UFUNCTION(BlueprintPure, Category="UI")
	UUserWidget* GetHUDWidgetInstance() const { return HUDWidgetInstance; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	UFUNCTION()
	void HandlePlayerDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<UUserWidget> HUDWidgetInstance;
};
