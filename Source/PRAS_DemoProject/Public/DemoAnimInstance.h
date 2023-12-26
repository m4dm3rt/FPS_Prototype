// Designed by Hitman's Store 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DemoAnimInstance.generated.h"

class APRAS_DemoProjectCharacter;

UCLASS(Blueprintable)
class PRAS_DEMOPROJECT_API UDemoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pose")
	UAnimSequence* BasePose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pose")
	FRotator LookRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pose")
	FVector WeaponOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pose")
	FTransform PointAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pose")
	FVector PivotOffset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pose")
	FVector GunPivotOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pose")
	FTransform EditingOffset;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	APRAS_DemoProjectCharacter* Character;
};