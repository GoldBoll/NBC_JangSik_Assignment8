#include "Characters/VOIDZombieCharacter.h"

AVOIDZombieCharacter::AVOIDZombieCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVOIDZombieCharacter::ReactToNoise(const FVector& NoiseLocation, float NoiseRadius)
{
	// Blackboard에 NoiseLocation 기록 → BT가 이를 소비해 추적
}
