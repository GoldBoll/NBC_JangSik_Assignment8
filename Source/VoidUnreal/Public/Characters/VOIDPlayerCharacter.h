#pragma once

#include "CoreMinimal.h"
#include "Characters/VOIDBaseCharacter.h"
#include "InputActionValue.h"
#include "VOIDPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UVOIDInventoryComponent;
class UVOIDNoiseComponent;
class UVOIDDebuffComponent;
class UInputMappingContext;
class UInputAction;
class UAIPerceptionStimuliSourceComponent;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDPlayerCharacter : public AVOIDBaseCharacter
{
	GENERATED_BODY()

public:
	AVOIDPlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);

	// TPS 숄더뷰 카메라 리그 (Over-the-Shoulder, 오른쪽 어깨 기본)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// Void 핵심 컴포넌트 (무게·소음·디버프)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDNoiseComponent> NoiseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UVOIDDebuffComponent> DebuffComponent;

	// 옵션 2: AIPerception 청각 시스템에 자동 등록 (좀비가 사격·발소리를 듣게 함)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	// Enhanced Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> FireAction;
};
