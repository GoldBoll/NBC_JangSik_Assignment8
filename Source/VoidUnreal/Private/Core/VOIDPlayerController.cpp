#include "Core/VOIDPlayerController.h"
#include "Blueprint/UserWidget.h"

AVOIDPlayerController::AVOIDPlayerController()
{
}

void AVOIDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
}

void AVOIDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
