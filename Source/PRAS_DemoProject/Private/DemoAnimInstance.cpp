// Designed by Hitman's Store 2022

#include "DemoAnimInstance.h"
#include "PRAS_DemoProjectCharacter.h"

void UDemoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<APRAS_DemoProjectCharacter>(TryGetPawnOwner());
}