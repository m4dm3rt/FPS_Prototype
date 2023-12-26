// Designed by Hitman's Store 2022

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "PRAS_DemoProjectCharacter.generated.h"

class UDemoAnimInstance;

UCLASS(config=Game)
class APRAS_DemoProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APRAS_DemoProjectCharacter();

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FPCamera; }
protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Turn(float Rate);
	void LookUp(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeapon() { return CurrentWeapon; };
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void OnFirePressed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void OnFireReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void OnWeaponEquipped(const FRecoilAnimData Data, const float Rate = 0.0f, const int Bursts = 0);

	UFUNCTION(BlueprintImplementableEvent, Category = "Input")
	void OnFireModeChanged(EFireMode_PRAS NewMode);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void Aim();
	UFUNCTION(Server, Unreliable, Category = "Net")
	void Server_Aim(bool Flag);

	UFUNCTION(BlueprintCallable)
	void EquipWeapon();
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ChangeWeapon();
	
	void UnequipWeapon();
	UFUNCTION(Server, Reliable, Category = "Net")
	void Server_Unequip(int WeaponIndex);
	UFUNCTION(NetMulticast, Reliable, Category = "Net")
	void Multicast_Unequip();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ChangeFireMode();

	UFUNCTION(Server, Reliable, Category = "Net")
	void Server_ChangeFireMode(EFireMode_PRAS Mode);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void FirePressed();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void FireReleased();

	UFUNCTION()
	void Fire();

	void HandleRecoil(bool bPlay);
	UFUNCTION(Server, Unreliable, Category = "Net")
	void Server_HandleRecoil(bool bPlay);
	UFUNCTION(NetMulticast, Unreliable, Category = "Net")
	void Multicast_FireAnim();

	UFUNCTION()
	void PlayCameraShake();
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UnequipMontage;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FPCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil Animation", meta = (AllowPrivateAccess="true"))
	URecoilAnimationComponent* RecoilAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Firing")
	TEnumAsByte<EFireMode_PRAS> FireMode;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<AWeapon>> WeaponTemplates;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float FOV = 90.f;
	
	UPROPERTY()
	TArray<AWeapon*> InitWeapons;

	UPROPERTY()
	AWeapon* CurrentWeapon = nullptr;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AimOffset", meta = (AllowPrivateAccess="true"))
	FRotator AimRotation;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	int CurrentGunIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess="true"))
	bool bAnimPlaying = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera Shake")
	bool bEnableShake = true;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Aiming")
	bool bAim = false;

	UPROPERTY()
	UDemoAnimInstance* AnimInstance;
	
	int BurstAmount = 0;

	FTimerHandle RecoilTimer;
};