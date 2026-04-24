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

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<UUserWidget> HUDWidgetInstance;
};
