// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"
#include "TrentinoAltoAdige/Interfaces/InteractionInterface.h"

#include "PlayerCharacter.generated.h"

class UInventoryComponent;
enum EHitDirection : int;
class UDamageComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
#define DEBUG_BUILD 1

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

	/*?-------------------|INPUT|--------------------*/
	//Default mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* ToggleWeaponAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* AttackAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* TargetAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* NextTargetAction;
		
	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* Parry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* InterAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* PauseMenuAction;
	
	/*?-------------------|COMPONENTS|--------------------*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components | Combat")
	UCombatSystemComponent* CombatSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components | Combat")
	UDamageComponent* DamageComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components | Inventory")
	UInventoryComponent* InventoryComponent;
	
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PauseGame();

	/*?-------------------|BLUEPRINT HOOKS|--------------------*/
	UFUNCTION(BlueprintImplementableEvent)
	void CameraShake();
	UFUNCTION(BlueprintImplementableEvent)
	void LerpCamToAttackPosition();
	UFUNCTION(BlueprintImplementableEvent)
	void ResetCam();
	UFUNCTION(BlueprintImplementableEvent)
	UCameraComponent* GetCamera();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay")
	void StartShopCameraAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void StopShopCameraAnimation();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsInShop = false;
	
	/*?-------------------|INTERFACES|--------------------*/
	virtual UDamageComponent* GetDamageComponent() const override {return DamageComponent;}
	virtual UDamageComponent* GetDamageComponent() override {return DamageComponent;}
	virtual UCombatSystemComponent* GetCombatSystemComponent() override {return CombatSystemComponent;}
	virtual UCombatSystemComponent* GetCombatSystemComponent() const override {return CombatSystemComponent;}
	virtual bool CanBeTargeted() override {return true;}
	virtual AWeaponBase* GetWeapon() const override {return CurrentWeapon;}
	virtual USkeletalMeshComponent* GetCharacterMesh() override {return GetMesh();}
	virtual void EquipWeapon() override {InternalEquipWeapon();}
	virtual void UnEquipWeapon() override {InternalUnEquipWeapon();}
	virtual bool IsWeaponEquipped() const override {return CombatSystemComponent->IsWeaponEquipped();}
	virtual void SetCanMove(bool val) override {bCanMove = val;}
	virtual void SnapToTarget() override
	{
		CombatSystemComponent->MoveToTarget();
		InternalSnapToTarget();
	}
	virtual void SetMovementToWalk() override { InternalSetMovementToWalk(); }
	virtual void ResetMovement() override  { InternalResetMovement(); }
	virtual bool IsParrying() override{return CombatSystemComponent->IsParrying();}
	virtual bool IsPerfectParrying() override {return CombatSystemComponent->IsPerfectParrying();}
	virtual ETeam GetTeam() override {return Player;}
	virtual void HandlePerfectParry() override {InternalHandlePerfectParry();}
	virtual void HandleParry() override {InternalHandleParry();}
	UFUNCTION(BlueprintNativeEvent)
	void InternalHandlePerfectParry();
	UFUNCTION(BlueprintNativeEvent)
	void InternalHandleParry();
	UFUNCTION(BlueprintImplementableEvent)
	void InternalSnapToTarget();
	void InternalNextTarget();
	void InternalStartParry();
	void InternalStopParry();
	UFUNCTION(BlueprintImplementableEvent)
	void InternalSetMovementToWalk();
	UFUNCTION(BlueprintImplementableEvent)
	void InternalResetMovement();
	UFUNCTION(BlueprintCallable)
	void ResetPlayerMovement();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanMove = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsSprinting = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanSprint = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsPaused = false;	
	
	/*?-------------------|WEAPON | VARS|--------------------*/
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;
	
	/*?-------------------|WEAPON | FUNCTIONS|--------------------*/
	UFUNCTION(BlueprintCallable)
	void ToggleWeapon();

public:	
	UFUNCTION(BlueprintCallable)
	void InternalEquipWeapon();
protected:
	
	UFUNCTION(BlueprintCallable)
	void InternalUnEquipWeapon();
	
	UFUNCTION(BlueprintCallable)
	void InternalTarget();

	void InternalAttack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponHoldingType WeaponHolding;
	
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> ShockwaveVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* ParryVoiceSound;
	
	UFUNCTION()
	void OnDeath();
	UFUNCTION()
	void OnDamageResponse(EHitDirection HitResponse);
		
	/*?-------------------|Interaction|--------------------*/
	void Interact();
	UFUNCTION(BlueprintCallable)
	void InternalStopInteract();
	void CheckForInteraction();
	void StopInteract();
	float InteractionCheckFreq = 0.15f;
	FTimerHandle InteractionTimer;
	IInteractionInterface* CurrentInteractable;
	IInteractionInterface* ActiveInteractionSession;
	
	/*?-------------------|ANIMATIONS|--------------------*/
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Animations | Weapon")
	TObjectPtr<UAnimMontage> ParryMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> FrontHitMontage;
		
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ParryResponse;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations | Weapon")
	TObjectPtr<UAnimMontage> EquipFromBackWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations | Weapon")
	TObjectPtr<UAnimMontage> UnEquipFromHandWeapon;
};
