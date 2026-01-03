// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatSystemComponent.generated.h"


class AWeaponBase;
class AEnemyBase;
class ICombatInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRENTINOALTOADIGE_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatSystemComponent();

	/*?-------------------|ATTACKS | FUNCTIONS|--------------------*/
	UFUNCTION(BlueprintPure)
	bool IsAttacking() const {return bIsAttacking;}
	void SaveCombo();
	void ResetCombo();
	void Attack();
	void PerformTrace();
	void ResetEnemiesHitThisAttack() {EnemiesHitThisAttack.Empty();}
	void ApplyHitStop(AActor* Actor, float Duration, float TimeDilation) const;
	UFUNCTION(BlueprintCallable)
	bool IsParrying() const {return bIsParrying;}
	float CalculateDamage(float WeaponBaseDamage) const;
	/*?-------------------|ATTACKS | FUNCTIONS|--------------------*/
	
	/*?-------------------|TARGETING | FUNCTIONS|--------------------*/
	void Target();
	bool IsTargeting() const {return bIsTargeting;}
    private: void StartTarget();
	private: void StopTarget();
	public:  void NextTarget();
	/*?-------------------|TARGETING | FUNCTIONS|--------------------*/
	 
	/*?-------------------|Weapon Equipping | FUNCTIONS|--------------------*/
	void EquipWeapon(FName InSocket, UAnimMontage* EquipMontage);
	void UnEquipWeapon(FName InSocket, UAnimMontage* UnEquipMontage);
	UFUNCTION(BlueprintCallable)
	const bool& IsWeaponEquipped() const  { return bIsWeaponEquipped;  }
	UFUNCTION(BlueprintCallable)
	const bool& IsEquippingWeapon() const { return bIsEquippingWeapon; }
	/*?-------------------|Weapon Equipping | FUNCTIONS|--------------------*/

	/*?-------------------|Weapon Leveling | FUNCTIONS|--------------------*/
	const int32& GetWeaponLevel() const {return CurrentWeaponLevel;} 
	UFUNCTION(BlueprintCallable)
	void UpgradeWeapon(/*!SOSTITUIRE CON INVENTORYCOMPONENT*/int32 NumberOfShards);
	void UpdateWeaponMesh(int32 WeaponLevel);
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	int32 GetUpgradeCostForLevel(int32 Level) const;
	/*?-------------------|Weapon Leveling | FUNCTIONS|--------------------*/
public:
	/*?-------------------|Parry | FUNCTIONS|--------------------*/
	void StartParry();
	void EndParry();
	/*?-------------------|Parry | FUNCTIONS|--------------------*/
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Combat | HitStop")
	float HitStopDuration = 0.1f;
	
	UPROPERTY(EditAnywhere, Category = "Combat | HitStop")
	float HitStopTimeDilation = 0.12f;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> CurrentHitActor;

private:
	/*?-------------------|REFs|--------------------*/
	ICombatInterface* OwnerRef;	//? Combat Owner Ref
	TObjectPtr<class APlayerCharacter> PlayerOwnerRef; //? Ref of just the player
	/*?-------------------|REFs|--------------------*/
	
	/*?-------------------|ATTACK | VARS|--------------------*/
	int32 AttackIndex = 0;		//? Index of the current attack in the array
	bool bIsAttacking = false;	//? Flag if the character is attacking or not
	bool bSaveCombo = false;	//? Flag to save the combo in the Montages
	bool bIsParrying = false;
	TSet<TObjectPtr<AActor>> EnemiesHitThisAttack;	//? Set of Actor Pointers hit this attack, so we don't hit the same Actor multiple times
	UAnimMontage* CurrentAttackMontage;
	float DefaultMovementSpeed = 0.f;
	/*?-------------------|ATTACK | VARS|--------------------*/
	
	/*?-------------------|TARGETING | VARS|--------------------*/
	TArray<TObjectPtr<AActor>> TargetActors;
	bool bIsTargeting = false;
	TObjectPtr<AActor> CurrentTargetActor;
	FTimerHandle LerpToTargetActorTimer;
	int32 TargetIndex = 0;
	/*?-------------------|TARGETING | VARS|--------------------*/
	
	/*?-------------------|Equipping | VARS|--------------------*/
	bool bIsEquippingWeapon = false;
	bool bIsWeaponEquipped = false;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnEquipped);
	UPROPERTY(BlueprintAssignable)
	FOnWeaponUnEquipped OnWeaponUnEquipped;
	FTimerHandle InterpolationTimer;
	float ElapsedTime;
	UPROPERTY(EditAnywhere, Category = "Equipping")
	float TotalDuration = 1.f;
	/*?-------------------|Equipping | VARS|--------------------*/
	
	/*?-------------------|Weapon Leveling | VARS|--------------------*/
	int32 CurrentWeaponLevel = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon") 
	TArray<USkeletalMesh*> WeaponMeshes;
	int32 BaseCostForUpgrade = 4;
	int32 MaxWeaponLevel = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades")
	float CostMultiplier = 2.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades")
	float DamageMultiplierPerLevel = 0.1f;
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponUpgraded, int32, NewLevel);
	UPROPERTY(BlueprintAssignable)
	FOnWeaponUpgraded OnWeaponUpgraded;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFailedUpgrade, FString, ErrorMessage);
	UPROPERTY(BlueprintAssignable)
	FOnWeaponFailedUpgrade OnWeaponFailedUpgrade;
private:
	/*?-------------------|Weapon Leveling | VARS|--------------------*/

	//TEMP
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HitReactionMontage;

	/*?-------------------|Anims|--------------------*/
	TObjectPtr<class UCharacterAnimInstance> AnimInstance;
	/*?-------------------|Anims|--------------------*/
	
	/*?-------------------|VFX|--------------------*/
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class UNiagaraSystem> HitVFX;
	/*?-------------------|VFX|--------------------*/
	
	/*?-------------------|SOUND|--------------------*/
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> AttackSound;
	/*?-------------------|SOUND|--------------------*/
};
