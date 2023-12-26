// Designed by Hitman's Store 2022

#include "PRAS_DemoProjectCharacter.h"

#include "DemoAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

APRAS_DemoProjectCharacter::APRAS_DemoProjectCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FPCamera->SetupAttachment(GetMesh(), TEXT("CameraSocket"));
	FPCamera->bEditableWhenInherited = true;
	FPCamera->bUsePawnControlRotation = true;

	RecoilAnimation = CreateDefaultSubobject<URecoilAnimationComponent>(TEXT("RecoilAnimComp"));
}

void APRAS_DemoProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APRAS_DemoProjectCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APRAS_DemoProjectCharacter::FireReleased);
	PlayerInputComponent->BindAction("ChangeWeapon", IE_Pressed, this, &APRAS_DemoProjectCharacter::ChangeWeapon);
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &APRAS_DemoProjectCharacter::ChangeFireMode);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APRAS_DemoProjectCharacter::Aim);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APRAS_DemoProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APRAS_DemoProjectCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APRAS_DemoProjectCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APRAS_DemoProjectCharacter::LookUp);
}

void APRAS_DemoProjectCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInstance = Cast<UDemoAnimInstance>(GetMesh()->GetAnimInstance());
}

void APRAS_DemoProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->AddTickPrerequisiteActor(this);
	OnFireModeChanged(EFireMode_PRAS::Semi);
	
	for(int i = 0; i < WeaponTemplates.Num(); i++)
	{
		AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponTemplates[i], GetActorLocation(), GetActorRotation());
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("VB WeaponPivot"));
		
		NewWeapon->GunMesh->SetVisibility(false, true);
		NewWeapon->SetupAttachments();
		InitWeapons.Add(NewWeapon);
	}

	if(WeaponTemplates.Num() > 0)
	{
		CurrentWeapon = InitWeapons[0];
		EquipWeapon();
	}
	
	if(IsLocallyControlled())
	{
		FOnTimelineEvent Event;
    	Event.BindDynamic(this, &APRAS_DemoProjectCharacter::PlayCameraShake);
    	RecoilAnimation->AddEvent(0.02f, Event);

		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->SetFOV(FOV);
	}
}

void APRAS_DemoProjectCharacter::PlayCameraShake()
{
	if (IsLocallyControlled() && bEnableShake && CurrentWeapon && CurrentWeapon->CameraRecoilShake)
	{
		UGameplayStatics::GetPlayerCameraManager(this, 0)
			->StartCameraShake(CurrentWeapon->CameraRecoilShake);
	}
}

void APRAS_DemoProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APRAS_DemoProjectCharacter, AimRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APRAS_DemoProjectCharacter, CurrentGunIndex, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APRAS_DemoProjectCharacter, bAim, COND_SkipOwner);
}

void APRAS_DemoProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (HasAuthority() || IsLocallyControlled())
	{
		AimRotation = GetControlRotation();
	}
	
	if (AnimInstance)
	{
		FRotator Delta = AimRotation - GetActorRotation();
		Delta.Normalize();
		AnimInstance->LookRotation = Delta;
	}
}

void APRAS_DemoProjectCharacter::ChangeWeapon()
{
	if(!CurrentWeapon || bAnimPlaying)
	{
		return;
	}

	const int LastIndex = CurrentGunIndex;
	CurrentGunIndex++;

	if(CurrentGunIndex > InitWeapons.Num() - 1)
	{
		CurrentGunIndex = 0;
	}

	if(!InitWeapons[CurrentGunIndex])
	{
		CurrentGunIndex = LastIndex;
		return;
	}

	UnequipWeapon();
}

void APRAS_DemoProjectCharacter::UnequipWeapon()
{
	if(UnequipMontage)
	{
		PlayAnimMontage(UnequipMontage);
	}

	bAim = false;

	if(IsLocallyControlled() && !HasAuthority())
	{
		Server_Unequip(CurrentGunIndex);
	}
	else if(HasAuthority())
	{
		Multicast_Unequip();
	}
}

void APRAS_DemoProjectCharacter::Server_Unequip_Implementation(int WeaponIndex)
{
	if(HasAuthority())
	{
		CurrentGunIndex = WeaponIndex;
		const auto Gun = InitWeapons[CurrentGunIndex];
		OnWeaponEquipped(Gun->RecoilAnimData, Gun->FireRate, Gun->Burst);
		bAim = false;
		
		Multicast_Unequip();
	}
}

void APRAS_DemoProjectCharacter::Multicast_Unequip_Implementation()
{
	if(!IsLocallyControlled())
	{
		if(UnequipMontage)
		{
			PlayAnimMontage(UnequipMontage);
		}
	}
}

void APRAS_DemoProjectCharacter::EquipWeapon()
{
	CurrentWeapon->GunMesh->SetVisibility(false, false);
	CurrentWeapon->SetAttachmentsVisibility(false);
	CurrentWeapon = InitWeapons[CurrentGunIndex];
	CurrentWeapon->GunMesh->SetVisibility(true, false);
	CurrentWeapon->SetAttachmentsVisibility(true);

	FVector Offset = CurrentWeapon->GunMesh->GetSocketTransform(TEXT("PivotPoint"),RTS_Component).GetLocation();
	FVector GunPivotOffset = CurrentWeapon->GunPivotOffset;
	CurrentWeapon->SetActorRelativeLocation(-Offset - GunPivotOffset);

	if(AnimInstance)
	{
		AnimInstance->BasePose = CurrentWeapon->BasePose;
		AnimInstance->WeaponOffset = CurrentWeapon->PositionOffset;
		AnimInstance->PointAim = CurrentWeapon->PointAim;
		AnimInstance->PivotOffset = Offset;
		AnimInstance->EditingOffset = CurrentWeapon->EditingOffset;
		AnimInstance->GunPivotOffset = GunPivotOffset;
	}

	if(EquipMontage)
	{
		PlayAnimMontage(EquipMontage);
	}

	if(IsLocallyControlled() || HasAuthority())
	{
		OnWeaponEquipped(CurrentWeapon->RecoilAnimData, CurrentWeapon->FireRate, CurrentWeapon->Burst);
		FireMode = Semi;
		OnFireModeChanged(FireMode);
		BurstAmount = CurrentWeapon->Burst;
	}
}

void APRAS_DemoProjectCharacter::ChangeFireMode()
{
	switch (FireMode)
	{
	case Semi:

		if(CurrentWeapon->bBursts)
		{
			FireMode = Burst;
		}
		else if(CurrentWeapon->bFullAuto)
		{
			FireMode = Auto;
		}
		break;

	case Burst:
		if(CurrentWeapon->bFullAuto)
		{
			FireMode = Auto;
		}
		else
		{
			FireMode = Semi;
		}
		break;

	case Auto:
		FireMode = Semi;
		break;
	}

	OnFireModeChanged(FireMode);

	if(IsLocallyControlled() && !HasAuthority())
	{
		Server_ChangeFireMode(FireMode);
	}
}

void APRAS_DemoProjectCharacter::Server_ChangeFireMode_Implementation(EFireMode_PRAS Mode)
{
	if(HasAuthority())
	{
		FireMode = Mode;
		OnFireModeChanged(FireMode);
	}
}

void APRAS_DemoProjectCharacter::FirePressed()
{
	float TimerError = (60.f / CurrentWeapon->FireRate) / GetWorld()->GetDeltaSeconds() + 1;
	TimerError *= GetWorld()->GetDeltaSeconds();
	
	if (RecoilAnimation->GetDelta() > TimerError + 0.01f)
	{
		if(FireMode == Burst && BurstAmount == 0)
		{
			BurstAmount = CurrentWeapon->Burst;
		}
		
		Fire();
	}
}

void APRAS_DemoProjectCharacter::FireReleased()
{
	OnFireReleased();
	GetWorld()->GetTimerManager().ClearTimer(RecoilTimer);

	if(IsLocallyControlled() && !HasAuthority())
	{
		Server_HandleRecoil(false);
	}
}

void APRAS_DemoProjectCharacter::Fire()
{
	const bool bPlay = !(FireMode == Burst && BurstAmount == 0);
	HandleRecoil(bPlay);
	
	if(FireMode != Semi && bPlay)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, TEXT("Fire"));
		
		const float Delay = (60.f / FMath::Max(CurrentWeapon->FireRate, 0.001f));
		GetWorld()->GetTimerManager().SetTimer(RecoilTimer, Delegate, Delay, false);

		BurstAmount = FireMode == Burst ? BurstAmount - 1 : BurstAmount;
	}
}

void APRAS_DemoProjectCharacter::HandleRecoil(bool bPlay)
{
	if(bPlay)
	{
		OnFirePressed();
		CurrentWeapon->PlayFireAnim();
	}
	else
	{
		OnFireReleased();

		if(IsLocallyControlled())
		{
			GetWorld()->GetTimerManager().ClearTimer(RecoilTimer);
		}
	}

	if(IsLocallyControlled())
	{
		Server_HandleRecoil(bPlay);
	}
}

void APRAS_DemoProjectCharacter::Multicast_FireAnim_Implementation()
{
	if(!IsLocallyControlled())
	{
		CurrentWeapon->PlayFireAnim();
	}
}

void APRAS_DemoProjectCharacter::Server_HandleRecoil_Implementation(bool bPlay)
{
	if(HasAuthority())
	{
		if(!IsLocallyControlled())
		{
			HandleRecoil(bPlay);
		}

		if(bPlay)
		{
			Multicast_FireAnim();
		}
	}
}

void APRAS_DemoProjectCharacter::Aim()
{
	bAim = !bAim;

	if(IsLocallyControlled() && !HasAuthority())
	{
		Server_Aim(bAim);
	}
}

void APRAS_DemoProjectCharacter::Server_Aim_Implementation(bool Flag)
{
	if(HasAuthority())
	{
		bAim = Flag;
	}
}

void APRAS_DemoProjectCharacter::Turn(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate);
}

void APRAS_DemoProjectCharacter::LookUp(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate);
}

void APRAS_DemoProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APRAS_DemoProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}