#include "Characters/VOIDPlayerCharacter.h"

#include "VoidUnreal.h"  // ECC_Weapon 별칭
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "Components/VOIDInventoryComponent.h"
#include "Components/VOIDNoiseComponent.h"
#include "Components/VOIDDebuffComponent.h"
#include "Components/VOIDWeaponComponent.h"

#include "Items/VOIDItemInterface.h"
#include "Items/VOIDItemDataAsset.h"
#include "Items/VOIDPickupBase.h"
#include "Weapon/VOIDWeaponConfig.h"
#include "Items/VOIDVehiclePartSlot.h"
#include "Vehicle/VOIDVehicle.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

AVOIDPlayerCharacter::AVOIDPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// --- TPS 숄더뷰 카메라 리그 (Over-the-Shoulder, 오른쪽 어깨) ---
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// --- TPS 표준: 캐릭터가 컨트롤러 Yaw를 따라가도록 설정 ---
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = true;
	bUseControllerRotationRoll  = false;

	// --- 기본 걷기 속도 ---
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 450.0f;
	}

	// --- Void 핵심 시스템 3종 ---
	InventoryComponent = CreateDefaultSubobject<UVOIDInventoryComponent>(TEXT("InventoryComponent"));
	NoiseComponent     = CreateDefaultSubobject<UVOIDNoiseComponent>(TEXT("NoiseComponent"));
	DebuffComponent    = CreateDefaultSubobject<UVOIDDebuffComponent>(TEXT("DebuffComponent"));

	WeaponComp = CreateDefaultSubobject<UVOIDWeaponComponent>(TEXT("WeaponComp"));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->bAutoRegister = true;
	StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
}

void AVOIDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AVOIDPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickAds(DeltaTime);

	if (WeaponComp)
	{
		float PitchDelta = 0.f, YawDelta = 0.f;
		WeaponComp->TickRecoil(DeltaTime, PitchDelta, YawDelta);
		if (!FMath::IsNearlyZero(PitchDelta)) { AddControllerPitchInput(PitchDelta); }
		if (!FMath::IsNearlyZero(YawDelta))   { AddControllerYawInput(YawDelta); }
	}
}

void AVOIDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) { return; }

	if (MoveAction)     { EIC->BindAction(MoveAction,     ETriggerEvent::Triggered, this, &AVOIDPlayerCharacter::Move); }
	if (LookAction)     { EIC->BindAction(LookAction,     ETriggerEvent::Triggered, this, &AVOIDPlayerCharacter::Look); }
	if (InteractAction) { EIC->BindAction(InteractAction, ETriggerEvent::Started,   this, &AVOIDPlayerCharacter::Interact); }
	if (FireAction)     { EIC->BindAction(FireAction,     ETriggerEvent::Started,   this, &AVOIDPlayerCharacter::Fire); }
	if (AimAction)
	{
		EIC->BindAction(AimAction, ETriggerEvent::Started,   this, &AVOIDPlayerCharacter::OnAimStarted);
		EIC->BindAction(AimAction, ETriggerEvent::Completed, this, &AVOIDPlayerCharacter::OnAimCompleted);
	}
	if (SwitchToRifleAction)   { EIC->BindAction(SwitchToRifleAction,   ETriggerEvent::Started, this, &AVOIDPlayerCharacter::OnSwitchToRifle); }
	if (SwitchToShotgunAction) { EIC->BindAction(SwitchToShotgunAction, ETriggerEvent::Started, this, &AVOIDPlayerCharacter::OnSwitchToShotgun); }
}

void AVOIDPlayerCharacter::OnSwitchToRifle(const FInputActionValue& Value)
{
	if (RifleConfig) { EquipWeapon(RifleConfig); }
}

void AVOIDPlayerCharacter::OnSwitchToShotgun(const FInputActionValue& Value)
{
	if (ShotgunConfig) { EquipWeapon(ShotgunConfig); }
}

void AVOIDPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) { return; }

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AVOIDPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AVOIDPlayerCharacter::Interact(const FInputActionValue& Value)
{
	if (!InventoryComponent) { return; }

	// 정면 200cm Sphere Sweep — 가장 가까운 인터랙터블 1개
	const FVector Start = GetActorLocation();
	const FVector End   = Start + GetActorForwardVector() * 200.0f;
	const float SphereRadius = 80.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = false;

	// 헬기 본체 메시(차량 자체)는 Block 응답이라 Sweep이 거기서 끊긴다 → 무시 처리
	for (TActorIterator<AVOIDVehicle> It(GetWorld()); It; ++It)
	{
		Params.AddIgnoredActor(*It);
	}

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(
		Hits, Start, End, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(SphereRadius), Params);

#if !(UE_BUILD_SHIPPING)
	UE_LOG(LogTemp, Warning, TEXT("[Interact] Sweep hits=%d"), Hits.Num());
	for (const FHitResult& H : Hits)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Interact]   Hit: %s (Component=%s)"),
			*GetNameSafe(H.GetActor()),
			*GetNameSafe(H.GetComponent()));
	}
#endif

	AActor* Best = nullptr;
	float BestDistSq = FLT_MAX;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) { continue; }

		// 후보 우선순위: Slot 설치 → Vehicle 시동 → Item 픽업
		const bool bSlot    = HitActor->Implements<UVOIDVehiclePartSlot>();
		const bool bVehicle = HitActor->IsA(AVOIDVehicle::StaticClass());
		const bool bItem    = HitActor->Implements<UVOIDItemInterface>();
		const bool bStaticDecor = HitActor->IsA(AStaticMeshActor::StaticClass());
		if (bStaticDecor) { continue; } // 헬기 데코 메시 등은 인터랙트 후보 아님
		if (!bSlot && !bVehicle && !bItem) { continue; }

		const float DistSq = FVector::DistSquared(Start, HitActor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = HitActor;
		}
	}

#if !(UE_BUILD_SHIPPING)
	DrawDebugSphere(GetWorld(), End, SphereRadius, 12,
		Best ? FColor::Green : FColor::Yellow, false, 0.5f);
#endif

	if (!Best) { return; }

	// 슬롯 분기: TODO Day 5 — InventoryComponent::FindPartByType 추가 후 정확한 부품 매칭으로 교체
	if (Best->Implements<UVOIDVehiclePartSlot>())
	{
		const EVOIDVehiclePartType SlotType = IVOIDVehiclePartSlot::Execute_GetRequiredPartType(Best);
		UVOIDItemDataAsset* CandidatePart = InventoryComponent ? InventoryComponent->FindPartByType(SlotType) : nullptr;

		if (!CandidatePart)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Interact] InstallPart Slot=%s FAIL — 인벤토리에 부품 없음"),
				*UEnum::GetValueAsString(SlotType));
			return;
		}

		const float WeightBefore = InventoryComponent ? InventoryComponent->GetTotalWeight() : 0.f;
		const bool bOk = IVOIDVehiclePartSlot::Execute_TryInstallPart(Best, CandidatePart, this);
		const float WeightAfter  = InventoryComponent ? InventoryComponent->GetTotalWeight() : 0.f;

		UE_LOG(LogTemp, Warning, TEXT("[Interact] InstallPart Slot=%s Part=%s Result=%s  Weight %.2f → %.2f / %.2f"),
			*UEnum::GetValueAsString(SlotType),
			*CandidatePart->GetName(),
			bOk ? TEXT("OK") : TEXT("FAIL"),
			WeightBefore, WeightAfter,
			InventoryComponent ? InventoryComponent->GetMaxCarry() : 0.f);
		return;
	}

	if (auto* Vehicle = Cast<AVOIDVehicle>(Best))
	{
		const bool bStarted = Vehicle->TryStartEngine(this);
		UE_LOG(LogTemp, Display, TEXT("[Interact] StartEngine Result=%s"),
			bStarted ? TEXT("OK") : TEXT("FAIL (repair incomplete)"));
		return;
	}

	if (!Best->Implements<UVOIDItemInterface>()) { return; }

	UVOIDItemDataAsset* Data = IVOIDItemInterface::Execute_GetItemData(Best);
	if (!Data) { return; }

	// TODO: AVOIDPickupBase::Quantity를 인터페이스에 노출하면 1개 고정 제거
	if (!InventoryComponent->TryAddItem(Data, 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("[VOID Interact] %s 인벤토리 가득 (%.1f / %.1f)"),
			*Best->GetName(), InventoryComponent->GetTotalWeight(), InventoryComponent->GetMaxCarry());
		return;
	}

	IVOIDItemInterface::Execute_OnPickedUp(Best, this);

	if (NoiseComponent)
	{
		const float WeightRatio = InventoryComponent->GetWeightRatio();
		const float Multiplier = 1.0f + FMath::Clamp(WeightRatio, 0.0f, 1.0f) * NoiseWeightFactor;
		NoiseComponent->EmitNoise(EVOIDNoiseSource::Pickup, Multiplier);
	}

	Best->Destroy();
}

void AVOIDPlayerCharacter::Fire(const FInputActionValue& Value)
{
	if (!IsValid(WeaponComp) || !IsValid(FollowCamera)) { return; }

	const FVector  MuzzleLoc = FollowCamera->GetComponentLocation();
	const FRotator AimRot    = FollowCamera->GetComponentRotation();
	const float    SpreadMul = bIsAiming ? 0.3f : 1.f;

	const bool bFired = WeaponComp->TryFire(MuzzleLoc, AimRot, this, SpreadMul);

	if (bFired && NoiseComponent)
	{
		const float WeightRatio = InventoryComponent ? InventoryComponent->GetWeightRatio() : 0.0f;
		const float Multiplier = 1.0f + FMath::Clamp(WeightRatio, 0.0f, 1.0f) * NoiseWeightFactor;
		NoiseComponent->EmitNoise(EVOIDNoiseSource::Gunshot, Multiplier);
	}
}


void AVOIDPlayerCharacter::EquipWeapon(UVOIDWeaponConfig* NewWeapon)
{
	if (WeaponComp)
	{
		WeaponComp->EquipWeapon(NewWeapon);
	}
}

void AVOIDPlayerCharacter::TickAds(float DeltaTime)
{
	if (!IsValid(FollowCamera) || !IsValid(CameraBoom)) { return; }

	const float TargetFOV = bIsAiming ? AdsFOV : HipFOV;
	FollowCamera->SetFieldOfView(
		FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, AdsBlendSpeed));

	const float TargetArm = bIsAiming ? AdsArmLength : HipArmLength;
	CameraBoom->TargetArmLength =
		FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArm, DeltaTime, AdsBlendSpeed);

	// 무게·ADS 감속을 곱셈 합성 — 단일 지점 갱신
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		const float WeightRatio = InventoryComponent ? FMath::Clamp(InventoryComponent->GetWeightRatio(), 0.f, 1.f) : 0.f;
		const float WeightFactor = 1.f - WeightRatio * WeightSpeedPenalty;
		const float AdsMul = bIsAiming ? AdsMoveMultiplier : 1.f;
		Move->MaxWalkSpeed = BaseWalkSpeed * WeightFactor * AdsMul;
	}
}
