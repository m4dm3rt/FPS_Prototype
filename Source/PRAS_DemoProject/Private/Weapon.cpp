// Designed by Hitman's Store 2022

#include "Weapon.h"

AWeapon::AWeapon()
{
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GunMesh->bEditableWhenInherited = true;
	GunMesh->SetupAttachment(Pivot);
}

void AWeapon::SetupAttachments()
{
	for(int i = 0; i < Attachments.Num(); i++)
	{
		for(auto Template : Attachments[i].GroupTemplates)
		{
			ABaseAttachment* Attachment = nullptr;

			if (Template)
			{
				Attachment = GetWorld()->SpawnActor<ABaseAttachment>(Template, GetActorLocation(), GetActorRotation());
				Attachment->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				Attachment->SetActorRelativeLocation(Attachment->ParentTransform.GetLocation());
				Attachment->SetActorRelativeRotation(Attachment->ParentTransform.GetRotation());
				Attachment->SetVisibility(false);
			}

			Attachments[i].Group.Add(Attachment);
		}
	}
}

void AWeapon::PlayFireAnim() const
{
	if(FireMontage)
	{
		GunMesh->PlayAnimation(FireMontage, false);
	}
}

void AWeapon::EditAttachments(int Index)
{
	if(Index > Attachments.Num() - 1)
	{
		return;
	}
	
	auto& Group = Attachments[Index];
	Group.CycleGroup();
}

FInputScale_PRAS AWeapon::GetAnimScaleFromGroup(int Index)
{
	FInputScale_PRAS NewScale;
	if(Index > Attachments.Num() - 1)
	{
		return NewScale;
	}
	
	auto& Group = Attachments[Index];
	if(const auto Attachment = Group.Group[Group.Index])
	{
		NewScale = Attachment->RecoilAnimScale;
	}

	return NewScale;
}

void AWeapon::SetAttachmentsVisibility(bool bVisibility)
{
	for(auto Group : Attachments)
	{
		Group.SetVisibility(bVisibility);
	}
}
