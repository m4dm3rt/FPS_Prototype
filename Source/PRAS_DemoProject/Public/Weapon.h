// Designed by Hitman's Store 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseAttachment.h"
#include "GameFramework/Actor.h"
#include "RecoilAnimationComponent.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FAttachmentGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachments")
	TArray<TSubclassOf<ABaseAttachment>> GroupTemplates;

	UPROPERTY(BlueprintReadOnly, Category = "Attachments")
	TArray<ABaseAttachment*> Group;

	UPROPERTY(BlueprintReadOnly, Category = "Attachments")
	int Index = 0;

	FInputScale_PRAS GetInputScale()
	{
		FInputScale_PRAS InputScale_Pras;

		if(Group.Num() == 0)
		{
			return InputScale_Pras;
		}

		if(const auto Attachment = Group[Index])
		{
			InputScale_Pras = Attachment->RecoilAnimScale;
		}
		
		return InputScale_Pras;
	}

	void CycleGroup()
	{
		SetVisibility(false);
		Index++;
		Index = Index > Group.Num() - 1 ? 0 : Index;
		SetVisibility(true);
	}

	void SetVisibility(bool bVisible)
	{
		if(!Group[Index])
		{
			return;
		}
		
		Group[Index]->SetVisibility(bVisible);
	}
};

UCLASS(BlueprintType)
class PRAS_DEMOPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
	AWeapon();

	UPROPERTY()
	USceneComponent* Pivot;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireMontage;

public:
	void PlayFireAnim() const;

	UFUNCTION(BlueprintCallable)
	void EditAttachments(int Index);
	UFUNCTION(BlueprintCallable)
	FInputScale_PRAS GetAnimScaleFromGroup(int Index);
	void SetAttachmentsVisibility(bool bVisibility);
	void SetupAttachments();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimSequence* BasePose;

	// Gun offset in mesh space
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FVector PositionOffset = FVector::ZeroVector;
	
	// Gun offset in mesh space
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FVector GunPivotOffset = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FTransform PointAim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FTransform EditingOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FRecoilAnimData RecoilAnimData;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float FireRate = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	int Burst = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	bool bFullAuto = false;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	bool bBursts = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Animation")
	TSubclassOf<UCameraShakeBase> CameraRecoilShake;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachments")
	TArray<FAttachmentGroup> Attachments;
};