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

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"

AVOIDPlayerCharacter::AVOIDPlayerCharacter()
{
	// --- TPS 숄더뷰 카메라 리그 (Over-the-Shoulder, 오른쪽 어깨) ---
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f);    // 오른쪽 어깨 오프셋
	CameraBoom->bUsePawnControlRotation = true;                 // SpringArm이 컨트롤러 회전 따라감
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 15.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;              // SpringArm이 이미 회전 처리

	// --- TPS 표준: 캐릭터가 컨트롤러 Yaw를 따라가도록 설정 ---
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = true;
	bUseControllerRotationRoll  = false;

	// --- 기본 걷기 속도 (Void 소음 시스템 전제로 보수적인 값) ---
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 450.0f;
	}

	// --- Void 핵심 시스템 3종 (시점 무관) ---
	InventoryComponent = CreateDefaultSubobject<UVOIDInventoryComponent>(TEXT("InventoryComponent"));
	NoiseComponent     = CreateDefaultSubobject<UVOIDNoiseComponent>(TEXT("NoiseComponent"));
	DebuffComponent    = CreateDefaultSubobject<UVOIDDebuffComponent>(TEXT("DebuffComponent"));

	// AIPerception 소음 발신원 등록 (좀비가 사격을 듣게 함)
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

void AVOIDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) { return; }

	if (MoveAction)     { EIC->BindAction(MoveAction,     ETriggerEvent::Triggered, this, &AVOIDPlayerCharacter::Move); }
	if (LookAction)     { EIC->BindAction(LookAction,     ETriggerEvent::Triggered, this, &AVOIDPlayerCharacter::Look); }
	if (InteractAction) { EIC->BindAction(InteractAction, ETriggerEvent::Started,   this, &AVOIDPlayerCharacter::Interact); }
	if (FireAction)     { EIC->BindAction(FireAction,     ETriggerEvent::Started,   this, &AVOIDPlayerCharacter::Fire); }
}

void AVOIDPlayerCharacter::Move(const FInputActionValue& Value)
{
	// 컨트롤러가 있어야 방향 계산이 가능
	if (!Controller) { return; }

	// Value는 Axis2D로 설정된 IA_Move의 입력값 (WASD)을 담고 있음
	// 예) (X=1, Y=0) → 전진 / (X=-1, Y=0) → 후진 / (X=0, Y=1) → 오른쪽 / (X=0, Y=-1) → 왼쪽
	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// 캐릭터가 바라보는 방향(정면)으로 X축 이동 (bUseControllerRotationYaw=true이므로 카메라 방향과 일치)
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		// 캐릭터의 오른쪽 방향으로 Y축 이동
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AVOIDPlayerCharacter::Look(const FInputActionValue& Value)
{
	// 마우스의 X, Y 움직임을 2D 축으로 가져옴
	const FVector2D LookInput = Value.Get<FVector2D>();

	// X는 좌우 회전 (Yaw), Y는 상하 회전 (Pitch)
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AVOIDPlayerCharacter::Interact(const FInputActionValue& Value)
{
	// Press E — 근접 아이템 픽업 (ItemInterface 호출)
}

void AVOIDPlayerCharacter::Fire(const FInputActionValue& Value)
{
	// 트레이스 시작점·끝점 계산 (카메라 기준)
	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector ForwardVector = FollowCamera->GetForwardVector();
	const FVector End = Start + (ForwardVector * 5000.f);  // 50m 사거리

	// 충돌 쿼리 파라미터 — 자기 자신 무시
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = false;

	// LineTrace 실행 (Weapon 커스텀 채널 — Visibility와 분리해서 사격 전용으로 사용)
	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Weapon,
		Params);

	// 피격 처리 — 죽은 캐릭터는 무시 (시체에 사격 시 로그 도배 방지)
	if (bHit)
	{
		if (auto* Target = Cast<AVOIDBaseCharacter>(Hit.GetActor()))
		{
			if (!Target->IsDead())
			{
				Target->ApplyDamage(25.f);
				UE_LOG(LogTemp, Warning, TEXT("Hit %s for 25 damage"), *Target->GetName());
			}
		}
	}

	// 소음 발생
	if (NoiseComponent)
		NoiseComponent->EmitNoise(EVOIDNoiseSource::Gunshot);

	// 디버그 시각화
	const FVector DebugEnd = bHit ? Hit.ImpactPoint : End;
	DrawDebugLine(
		GetWorld(),
		Start,
		DebugEnd,
		FColor::Red,
		false,
		2.f,
		0,
		1.f
		);
	
	if (bHit)
		DrawDebugSphere(
			GetWorld(),
			Hit.ImpactPoint,
			8.f,
			8,
			FColor::Yellow,
			false,
			2.f
			);
}
