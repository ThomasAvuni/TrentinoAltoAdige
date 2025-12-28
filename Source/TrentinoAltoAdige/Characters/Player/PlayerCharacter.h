// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"

#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;

UENUM(BlueprintType)
enum class EWeaponHoldingType : uint8
{
	None, 
	GreatSword UMETA(DisplayName = "Ranged Weapon"),
};

UCLASS()
class TRENTINOALTOADIGE_API APlayerCharacter : public ACharacter, public IGetComponentInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Default mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* ToggleWeaponAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* AttackAction;
	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Components | Combat")
	UCombatSystemComponent* CombatSystemComponent;
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//?Blueprint Hooks
	UFUNCTION(BlueprintImplementableEvent)
	void CameraShake();
	
	//?Interfaces
	virtual UCombatSystemComponent* GetCombatSystemComponent() const override {return CombatSystemComponent;}
	virtual AWeaponBase* GetWeapon() const override {return CurrentWeapon;}
	virtual USkeletalMeshComponent* GetCharacterMesh() override {return GetMesh();}
	virtual void EquipWeapon() override {InternalEquipWeapon();}
	virtual void UnEquipWeapon() override {InternalUnEquipWeapon();}
	virtual bool IsWeaponEquipped() const override {return bIsWeaponEquipped;}
	virtual void PerformCameraShake() override {CameraShake();}
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsSprinting = false;
	//?Weapon | Vars
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsWeaponEquipped = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCanEquipWeapon = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsEquippingWeapon = false;
	
	/*?Weapons | Functions*/
	UFUNCTION(BlueprintCallable)
	void ToggleWeapon();

	UFUNCTION(BlueprintCallable)
	void InternalEquipWeapon();
	
	UFUNCTION(BlueprintCallable)
	void InternalUnEquipWeapon();

	void Attack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponHoldingType WeaponHolding;
	
public:	AWeaponBase* GetCurrentWeapon() {return CurrentWeapon;}	
protected:
	
	/*?Animations*/
	TObjectPtr<UAnimInstance> AnimInstance;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnEquipped);
	UPROPERTY(BlueprintAssignable)
	FOnWeaponUnEquipped OnWeaponUnEquipped;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations | Weapon")
	TObjectPtr<UAnimMontage> EquipFromBackWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations | Weapon")
	TObjectPtr<UAnimMontage> UnEquipFromHandWeapon;
};

