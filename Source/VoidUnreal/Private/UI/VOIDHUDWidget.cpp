#include "UI/VOIDHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/VOIDHealthComponent.h"
#include "Components/VOIDInventoryComponent.h"
#include "Components/VOIDNoiseComponent.h"
#include "Components/VOIDWeaponComponent.h"
#include "Items/VOIDItemDataAsset.h"
#include "Core/VOIDGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Vehicle/VOIDVehicle.h"
#include "Weapon/VOIDWeaponConfig.h"

// 버튼 바인딩 + 레벨 이름으로 HUD 모드 결정 + GameState 위임자 등록 및 현재 값 초기 갱신
void UVOIDHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 OnClicked 바인딩
	if (Start)   { Start->OnClicked.AddUniqueDynamic(this, &UVOIDHUDWidget::OnClickStart); }
	if (ReStart) { ReStart->OnClicked.AddUniqueDynamic(this, &UVOIDHUDWidget::OnClickRestart); }
	if (Exit)    { Exit->OnClicked.AddUniqueDynamic(this, &UVOIDHUDWidget::OnClickExit); }

	// 현재 레벨에 따라 모드 자동 결정
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
	if      (LevelName == MenuLevelName.ToString())   SetHUDMode(EVOIDHUDMode::MainMenu);
	else if (LevelName == EscapeLevelName.ToString()) SetHUDMode(EVOIDHUDMode::GameClear);
	else                                              SetHUDMode(EVOIDHUDMode::InGame);

	UE_LOG(LogTemp, Display, TEXT("[HUD] Level=%s"), *LevelName);

	if (AVOIDGameState* GS = GetWorld() ? GetWorld()->GetGameState<AVOIDGameState>() : nullptr)
	{
		GS->OnScoreChanged.AddDynamic(this, &UVOIDHUDWidget::HandleScoreChanged);
		GS->OnWaveChanged.AddDynamic(this, &UVOIDHUDWidget::HandleWaveChanged);
		GS->OnTimeChanged.AddDynamic(this, &UVOIDHUDWidget::HandleTimeChanged);

		// 바인딩 이전에 발생한 Broadcast 를 보충 — 현재 값으로 초기 1회 갱신
		HandleScoreChanged(GS->GetCurrentScore());
		HandleWaveChanged(GS->GetCurrentWaveIndex());
		HandleTimeChanged(GS->GetRemainingTime());
	}
}

// HP/인벤/소음/디버프/무기 컴포넌트 위임자 일괄 바인딩
void UVOIDHUDWidget::BindToPlayer(APawn* PlayerPawn)
{
	if (!PlayerPawn) { return; }
	BoundPawn = PlayerPawn;

	if (UVOIDHealthComponent* HP = PlayerPawn->FindComponentByClass<UVOIDHealthComponent>())
	{
		HP->OnHealthChanged.AddDynamic(this, &UVOIDHUDWidget::HandleHealthChanged);
		HandleHealthChanged(HP->GetCurrentHealth());
	}
	if (UVOIDInventoryComponent* Inv = PlayerPawn->FindComponentByClass<UVOIDInventoryComponent>())
	{
		Inv->OnWeightChanged.AddDynamic(this, &UVOIDHUDWidget::HandleWeightChanged);
		HandleWeightChanged(Inv->GetTotalWeight(), Inv->GetMaxCarry());
		RefreshInventoryText();
	}
	if (UVOIDNoiseComponent* Noise = PlayerPawn->FindComponentByClass<UVOIDNoiseComponent>())
	{
		Noise->OnNoiseChanged.AddDynamic(this, &UVOIDHUDWidget::HandleNoiseChanged);
	}
	if (UVOIDDebuffComponent* Debuff = PlayerPawn->FindComponentByClass<UVOIDDebuffComponent>())
	{
		Debuff->OnDebuffUpdated.AddDynamic(this, &UVOIDHUDWidget::HandleDebuffUpdated);
	}
	if (UVOIDWeaponComponent* Weapon = PlayerPawn->FindComponentByClass<UVOIDWeaponComponent>())
	{
		Weapon->OnWeaponEquipped.AddDynamic(this, &UVOIDHUDWidget::HandleWeaponEquipped);
	}
}

// 차량 수리 슬롯 위임자 바인딩 + RepairText 초기화
void UVOIDHUDWidget::BindToVehicle(AVOIDVehicle* Vehicle)
{
	if (!Vehicle) { return; }
	BoundVehicle = Vehicle;
	InstalledCountCached = 0;

	Vehicle->OnSlotInstalled.AddDynamic(this, &UVOIDHUDWidget::HandleSlotInstalled);
	Vehicle->OnRepairComplete.AddDynamic(this, &UVOIDHUDWidget::HandleRepairComplete);

	if (RepairText)
	{
		RepairText->SetText(FText::FromString(TEXT("Repair : 0/3")));
	}
}

void UVOIDHUDWidget::HandleHealthChanged(float NewHealth)
{
	if (HealthBar) { HealthBar->SetPercent(NewHealth / 100.0f); }
}

void UVOIDHUDWidget::HandleWeightChanged(float TotalWeight, float MaxCarry)
{
	if (WeightBar && MaxCarry > 0.0f) { WeightBar->SetPercent(TotalWeight / MaxCarry); }
	RefreshInventoryText();
}

// 인벤토리 슬롯 전체를 문자열로 재구성해 Inventory 텍스트 갱신
void UVOIDHUDWidget::RefreshInventoryText()
{
	if (!Inventory) { return; }
	APawn* P = BoundPawn.Get();
	if (!P) { Inventory->SetText(FText::GetEmpty()); return; }
	UVOIDInventoryComponent* Inv = P->FindComponentByClass<UVOIDInventoryComponent>();
	if (!Inv) { Inventory->SetText(FText::GetEmpty()); return; }

	FString Out = TEXT("Inventory\n");
	for (const FVOIDInventorySlot& InvSlot : Inv->GetSlots())
	{
		if (!IsValid(InvSlot.ItemData)) continue;
		const FString Name = InvSlot.ItemData->DisplayName.IsEmpty()
			? InvSlot.ItemData->ItemID.ToString()
			: InvSlot.ItemData->DisplayName.ToString();
		Out += FString::Printf(TEXT(" - %s x%d\n"), *Name, InvSlot.Quantity);
	}
	Inventory->SetText(FText::FromString(Out));
}

void UVOIDHUDWidget::HandleNoiseChanged(float NoiseLevel)
{
	if (NoiseBar) { NoiseBar->SetPercent(NoiseLevel / 100.0f); }
}

// 활성 디버프를 한글 태그 문자열로 변환해 DebuffText 갱신
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
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), NewScore)));
	}
}

void UVOIDHUDWidget::HandleWaveChanged(int32 NewWave)
{
	if (WaveText) { WaveText->SetText(FText::Format(NSLOCTEXT("VOID", "Wave", "Wave {0}"), NewWave)); }
}

// 잔여 초를 MM:SS 형식으로 TimeText 갱신
void UVOIDHUDWidget::HandleTimeChanged(float RemainingTime)
{
	if (TimeText)
	{
		const int32 Min = FMath::FloorToInt(RemainingTime / 60.0f);
		const int32 Sec = FMath::FloorToInt(RemainingTime) % 60;
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Min, Sec)));
	}
}

// 무기 교체 시 WeaponIcon · WeaponNameText 갱신
void UVOIDHUDWidget::HandleWeaponEquipped(UVOIDWeaponConfig* NewWeapon)
{
	if (WeaponIcon)
	{
		if (NewWeapon && NewWeapon->Icon)
		{
			WeaponIcon->SetBrushFromTexture(NewWeapon->Icon);
			WeaponIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (WeaponNameText)
	{
		if (NewWeapon)
		{
			const FText Display = NewWeapon->DisplayName.IsEmpty()
				? FText::FromName(NewWeapon->WeaponId)
				: NewWeapon->DisplayName;
			WeaponNameText->SetText(Display);
		}
		else
		{
			WeaponNameText->SetText(FText::GetEmpty());
		}
	}
}

void UVOIDHUDWidget::HandleSlotInstalled(EVOIDVehiclePartType /*PartType*/)
{
	++InstalledCountCached;
	if (RepairText)
	{
		RepairText->SetText(FText::FromString(FString::Printf(TEXT("Repair : %d/3"), InstalledCountCached)));
	}
}

void UVOIDHUDWidget::HandleRepairComplete()
{
	if (RepairText)
	{
		RepairText->SetText(FText::FromString(TEXT("Repair : 3/3 - Press E to Start")));
	}
}

// 모드(MainMenu/InGame/GameOver/GameClear)에 따라 패널·버튼 가시성 + 입력 모드 전환
void UVOIDHUDWidget::SetHUDMode(EVOIDHUDMode NewMode)
{
	auto Toggle = [](UWidget* W, bool bShow)
	{
		if (!W) return;
		W->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	};

	const bool bIsMenu     = (NewMode == EVOIDHUDMode::MainMenu);
	const bool bIsInGame   = (NewMode == EVOIDHUDMode::InGame);
	const bool bIsOver     = (NewMode == EVOIDHUDMode::GameOver);
	const bool bIsClear    = (NewMode == EVOIDHUDMode::GameClear);
	const bool bShowPanel  = !bIsInGame; // Menu / Over / Clear 모두 Lv_1 사용

	Toggle(Lv_1, bShowPanel);
	Toggle(Lv_2, bIsInGame);
	Toggle(Lv_3, false); // Lv_3 사용 안 함

	// Lv_1 안 버튼 가시성
	Toggle(Start,   bIsMenu);                  // 메뉴에서만 Start
	Toggle(ReStart, bIsOver || bIsClear);      // 사망/클리어에서만 Restart
	Toggle(Exit,    bShowPanel);                // 메뉴/사망/클리어 모두 Exit

	// Lv_1 안 텍스트 가시성
	Toggle(GameOverText,   bIsOver);
	Toggle(GameOverText_1, bIsClear);

	// 인게임이면 마우스 숨김 + 게임 입력 / 메뉴면 마우스 + UI 입력
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		if (bShowPanel)
		{
			PC->SetShowMouseCursor(true);
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(TakeWidget());
			PC->SetInputMode(Mode);
		}
		else
		{
			PC->SetShowMouseCursor(false);
			FInputModeGameOnly Mode;
			PC->SetInputMode(Mode);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("[HUD] SetHUDMode=%d  Menu=%d InGame=%d Over=%d Clear=%d"),
		(int32)NewMode, bIsMenu, bIsInGame, bIsOver, bIsClear);
}

void UVOIDHUDWidget::OnClickStart()
{
	UGameplayStatics::OpenLevel(this, GameLevelName);
}

void UVOIDHUDWidget::OnClickRestart()
{
	UGameplayStatics::OpenLevel(this, GameLevelName);
}

void UVOIDHUDWidget::OnClickExit()
{
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
	const bool bIsMenu   = (LevelName == MenuLevelName.ToString());
	const bool bIsEscape = (LevelName == EscapeLevelName.ToString());

	if (bIsMenu || bIsEscape)
	{
		// 메인 메뉴 / 게임 클리어에서 Exit → 게임 종료
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
		}
	}
	else
	{
		// 인게임/오버에서 Exit → 메인 메뉴
		UGameplayStatics::OpenLevel(this, MenuLevelName);
	}
}
