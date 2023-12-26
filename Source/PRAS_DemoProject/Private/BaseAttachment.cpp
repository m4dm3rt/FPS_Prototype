// Designed by Hitman's Store 2022

#include "BaseAttachment.h"

ABaseAttachment::ABaseAttachment()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseAttachment::SetVisibility(bool bVisible)
{
	if(Mesh)
	{
		Mesh->SetVisibility(bVisible, false);
	}
}