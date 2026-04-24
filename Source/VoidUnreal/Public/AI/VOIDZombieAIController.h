#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "VOIDZombieAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;

UCLASS(Blueprintable)
class VOIDUNREAL_API AVOIDZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;
};
