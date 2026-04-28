#include "Components/VOIDWeaponComponent.h"

#include "VoidUnreal.h"  // ECC_Weapon 별칭
#include "Weapon/VOIDWeaponConfig.h"
#include "Characters/VOIDBaseCharacter.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UVOIDWeaponComponent::UVOIDWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVOIDWeaponComponent::EquipWeapon(UVOIDWeaponConfig* NewWeapon)
{
	CurrentWeapon = NewWeapon;
	PendingRecoilPitch = 0.f;
	PendingRecoilYaw   = 0.f;

	OnWeaponEquipped.Broadcast(NewWeapon);
}

bool UVOIDWeaponComponent::CanFireNow() const
{
	const UWorld* World = GetWorld();
	if (!World) { return false; }

	const float Now      = World->GetTimeSeconds();
	const float Interval = CurrentWeapon ? CurrentWeapon->FireInterval : 0.15f;
	return (Now - LastFireTime) >= Interval;
}

bool UVOIDWeaponComponent::TryFire(const FVector& MuzzleLocation, const FRotator& AimRotation,
                                   AActor* DamageInstigator, float SpreadMultiplier)
{
	UWorld* World = GetWorld();
	if (!World) { return false; }

	// CurrentWeapon이 GC됐거나 dangling이면 폴백 사용 — IsValid로 강제 체크
	UVOIDWeaponConfig* Weapon = IsValid(CurrentWeapon) ? CurrentWeapon : nullptr;
	if (!Weapon && CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] CurrentWeapon is dangling — clearing"));
		CurrentWeapon = nullptr;
	}

	const float Now      = World->GetTimeSeconds();
	const float Interval = Weapon ? Weapon->FireInterval : 0.15f;
	if (Now - LastFireTime < Interval) { return false; }
	LastFireTime = Now;

	const FVector Forward = AimRotation.Vector();

	// Weapon 미장착 시 25dmg/단발 폴백
	const int32 Pellets   = Weapon ? Weapon->PelletCount     : 1;
	const float SpreadDeg = Weapon ? Weapon->SpreadDegrees   : 0.f;
	const float Range     = Weapon ? Weapon->Range           : 5000.f;
	const float Damage    = Weapon ? Weapon->DamagePerPellet : 25.f;
	const float SpreadRad = FMath::DegreesToRadians(SpreadDeg * SpreadMultiplier);

	FVector  LastImpact   = MuzzleLocation + Forward * Range;
	AActor*  LastHitActor = nullptr;

	for (int32 i = 0; i < Pellets; ++i)
	{
		const FVector Dir = (Pellets == 1)
			? Forward
			: FMath::VRandCone(Forward, SpreadRad);

		FHitResult Hit;
		bool bHit = false;
		FireSinglePellet(MuzzleLocation, Dir, Range, Damage, DamageInstigator, Hit, bHit);

		if (bHit)
		{
			LastImpact   = Hit.ImpactPoint;
			LastHitActor = Hit.GetActor();
		}
	}

	if (Weapon)
	{
		PendingRecoilPitch += Weapon->RecoilPitch;
		const float J = Weapon->RecoilYawJitter;
		PendingRecoilYaw  += FMath::FRandRange(-J, J);
	}

	if (Weapon && Weapon->FireSound.IsValid())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Weapon->FireSound.Get(), MuzzleLocation);
	}

	OnWeaponFired.Broadcast(LastImpact, LastHitActor);
	return true;
}

void UVOIDWeaponComponent::FireSinglePellet(const FVector& Start, const FVector& Dir, float Range, float Damage,
                                            AActor* DamageInstigator, FHitResult& OutHit, bool& bOutHit)
{
	UWorld* World = GetWorld();
	if (!World) { bOutHit = false; return; }

	const FVector End = Start + Dir * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(VOIDFire), false, DamageInstigator);
	Params.bReturnPhysicalMaterial = false;

	bOutHit = World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Weapon, Params);

#if !(UE_BUILD_SHIPPING)
	if (bDrawWeaponDebug)
	{
		DrawDebugLine(World, Start, bOutHit ? OutHit.ImpactPoint : End,
			bOutHit ? FColor::Red : FColor::Green, false, 0.4f, 0, 1.f);
		if (bOutHit)
		{
			DrawDebugSphere(World, OutHit.ImpactPoint, 8.f, 8, FColor::Yellow, false, 0.4f);
		}
	}
#endif

	if (bOutHit)
	{
		ApplyHitDamage(OutHit, Damage, DamageInstigator);
	}
}

void UVOIDWeaponComponent::ApplyHitDamage(const FHitResult& Hit, float Damage, AActor* DamageInstigator)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor) { return; }

	// AVOIDBaseCharacter는 ApplyDamage 직접 호출 (시체 무시)
	if (auto* Target = Cast<AVOIDBaseCharacter>(HitActor))
	{
		if (!Target->IsDead())
		{
			Target->ApplyDamage(Damage);
			UE_LOG(LogTemp, Warning, TEXT("[Weapon] Hit %s for %.1f"), *Target->GetName(), Damage);
		}
		return;
	}

	// 일반 액터는 표준 PointDamage
	AController* InstigatorCtrl = nullptr;
	if (APawn* Pawn = Cast<APawn>(DamageInstigator))
	{
		InstigatorCtrl = Pawn->GetController();
	}
	const FVector Origin = DamageInstigator ? DamageInstigator->GetActorLocation() : FVector(Hit.TraceStart);
	UGameplayStatics::ApplyPointDamage(
		HitActor, Damage,
		(Hit.ImpactPoint - Origin).GetSafeNormal(),
		Hit, InstigatorCtrl, DamageInstigator, nullptr);
}

void UVOIDWeaponComponent::TickRecoil(float DeltaTime, float& OutPitchDelta, float& OutYawDelta)
{
	OutPitchDelta = 0.f;
	OutYawDelta   = 0.f;

	if (PendingRecoilPitch > KINDA_SMALL_NUMBER || FMath::Abs(PendingRecoilYaw) > KINDA_SMALL_NUMBER)
	{
		// AddControllerPitchInput는 음수가 위쪽
		OutPitchDelta = -PendingRecoilPitch;
		OutYawDelta   = PendingRecoilYaw;

		const float Recover = CurrentWeapon ? CurrentWeapon->RecoilRecoverPerSec : 4.f;
		PendingRecoilPitch = FMath::FInterpTo(PendingRecoilPitch, 0.f, DeltaTime, Recover);
		PendingRecoilYaw   = FMath::FInterpTo(PendingRecoilYaw,   0.f, DeltaTime, Recover);
	}
}
