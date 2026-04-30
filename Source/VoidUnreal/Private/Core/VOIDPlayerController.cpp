#include "Core/VOIDPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/VOIDHUDWidget.h"
#include "Components/VOIDHealthComponent.h"
#include "Core/VOIDGameMode.h"
#include "Kismet/GameplayStatics.h"

AVOIDPlayerController::AVOIDPlayerController()
{
}

// HUD 위젯 생성·뷰포트 추가, Pawn 존재 시 바로 BindToPlayer
void AVOIDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			if (UVOIDHUDWidget* HUD = Cast<UVOIDHUDWidget>(HUDWidgetInstance))
			{
				if (APawn* P = GetPawn())
				{
					HUD->BindToPlayer(P);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[VOID PC] BindToPlayer skipped (Pawn null) — will retry on Possess"));
				}
			}
		}
	}
}

// Pawn 교체 시 HUD 재바인딩 + HP OnDeath 델리게이트 등록
void AVOIDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UVOIDHUDWidget* HUD = Cast<UVOIDHUDWidget>(HUDWidgetInstance))
	{
		HUD->BindToPlayer(InPawn);
	}

	if (InPawn)
	{
		if (UVOIDHealthComponent* HP = InPawn->FindComponentByClass<UVOIDHealthComponent>())
		{
			HP->OnDeath.AddUniqueDynamic(this, &AVOIDPlayerController::HandlePlayerDeath);
		}
	}
}

void AVOIDPlayerController::HandlePlayerDeath()
{
	UE_LOG(LogTemp, Display, TEXT("[VOID PC] HandlePlayerDeath — notifying GameMode"));

	if (auto* GM = Cast<AVOIDGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->HandleGameOver();
	}
}

void AVOIDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
