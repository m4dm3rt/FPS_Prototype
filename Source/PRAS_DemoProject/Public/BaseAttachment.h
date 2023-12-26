// Designed by Hitman's Store 2022

#pragma once

#include "CoreMinimal.h"
#include "RecoilAnimationComponent.h"
#include "GameFramework/Actor.h"
#include "BaseAttachment.generated.h"

UCLASS(BlueprintType)
class PRAS_DEMOPROJECT_API ABaseAttachment : public AActor
{
	GENERATED_BODY()

public:
	ABaseAttachment();

	void SetVisibility(bool bVisible);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment Base")
	FTransform ParentTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil Anim Scale")
	FInputScale_PRAS RecoilAnimScale;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment Base")
	UStaticMeshComponent* Mesh;
};