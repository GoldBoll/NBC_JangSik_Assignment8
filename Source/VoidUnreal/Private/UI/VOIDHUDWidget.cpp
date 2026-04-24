#include "UI/VOIDHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VOIDHealthComponent.h"
#include "Components/VOIDInventoryComponent.h"
#include "Components/VOIDNoiseComponent.h"
#include "Core/VOIDGameState.h"
#include "Kismet/GameplayStatics.h"

void UVOIDHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AVOIDGameState* GS = GetWorld() ? GetWorld()->GetGameState<AVOIDGameState>() : nullptr)
	{
		GS->OnScoreChanged.AddDynamic(this, &UVOIDHUDWidget::HandleScoreChanged);
		GS->OnWaveChanged.AddDynamic(this, &UVOIDHUDWidget::HandleWaveChanged);
		GS->OnTimeChanged.AddDynamic(this, &UVOIDHUDWidget::HandleTimeChanged);
	}
}

void UVOIDHUDWidget::BindToPlayer(APawn* PlayerPawn)
{
	if (!PlayerPawn) { return; }
	BoundPawn = PlayerPawn;

	if (UVOIDHealthComponent* HP = PlayerPawn->FindComponentByClass<UVOIDHealthComponent>())
	{
		HP->OnHealthChanged.AddDynamic(this, &UVOIDHUDWidget::HandleHealthChanged);
	}
	if (UVOIDInventoryComponent* Inv = PlayerPawn->FindComponentByClass<UVOIDInventoryComponent>())
	{
		Inv->OnWeightChanged.AddDynamic(this, &UVOIDHUDWidget::HandleWeightChanged);
	}
	if (UVOIDNoiseComponent* Noise = PlayerPawn->FindComponentByClass<UVOIDNoiseComponent>())
	{
		Noise->OnNoiseChanged.AddDynamic(this, &UVOIDHUDWidget::HandleNoiseChanged);
	}
	if (UVOIDDebuffComponent* Debuff = PlayerPawn->FindComponentByClass<UVOIDDebuffComponent>())
	{
		Debuff->OnDebuffUpdated.AddDynamic(this, &UVOIDHUDWidget::HandleDebuffUpdated);
	}
}

void UVOIDHUDWidget::HandleHealthChanged(float NewHealth)
{
	if (HealthBar) { HealthBar->SetPercent(NewHealth / 100.0f); }
}

void UVOIDHUDWidget::HandleWeightChanged(float TotalWeight, float MaxCarry)
{
	if (WeightBar && MaxCarry > 0.0f) { WeightBar->SetPercent(TotalWeight / MaxCarry); }
}

void UVOIDHUDWidget::HandleNoiseChanged(float NoiseLevel)
{
	if (NoiseBar) { NoiseBar->SetPercent(NoiseLevel / 100.0f); }
}

void UVOIDHUDWidget::HandleDebuffUpdated(const TArray<EVOIDDebuffType>& ActiveDebuffs)
{
	if (!DebuffText) { return; }

	FString Text;
	for (EVOIDDebuffType Type : ActiveDebuffs)
	{
		switch (Type)
		{
		case EVOIDDebuffType::Bleeding:   Text += TEXT("[출혈] "); break;
		case EVOIDDebuffType::Fracture:   Text += TEXT("[골절] "); break;
		case EVOIDDebuffType::Overweight: Text += TEXT("[과적] "); break;
		default: break;
		}
	}
	DebuffText->SetText(FText::FromString(Text));
}

void UVOIDHUDWidget::HandleScoreChanged(int32 NewScore)
{
	if (ScoreText) { ScoreText->SetText(FText::AsNumber(NewScore)); }
}

void UVOIDHUDWidget::HandleWaveChanged(int32 NewWave)
{
	if (WaveText) { WaveText->SetText(FText::Format(NSLOCTEXT("VOID", "Wave", "Wave {0}"), NewWave)); }
}

void UVOIDHUDWidget::HandleTimeChanged(float RemainingTime)
{
	if (TimeText)
	{
		const int32 Min = FMath::FloorToInt(RemainingTime / 60.0f);
		const int32 Sec = FMath::FloorToInt(RemainingTime) % 60;
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Min, Sec)));
	}
}
