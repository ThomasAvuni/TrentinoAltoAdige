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
	
	/*?-------------------|TARGETING | FUNCTIONS|--------------------*/
	void Target();
	bool IsTargeting() const {return bIsTargeting;}

    private: void StartTarget();
	private: void StopTarget();
	public:  void NextTarget();
	 
	/*?-------------------|Weapon Equipping | FUNCTIONS|--------------------*/
	void EquipWeapon(FName InSocket, UAnimMontage* EquipMontage);
	void UnEquipWeapon(FName InSocket, UAnimMontage* UnEquipMontage);
	UFUNCTION(BlueprintCallable)
	const bool& IsWeaponEquipped() const  { return bIsWeaponEquipped;  }
	UFUNCTION(BlueprintCallable)
	const bool& IsEquippingWeapon() const { return bIsEquippingWeapon; }
	
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
	/*?-------------------|ATTACK | VARS|--------------------*/
	ICombatInterface* OwnerRef;	//? Combat Owner Ref
	TObjectPtr<class APlayerCharacter> PlayerOwnerRef; //? Ref of just the player
	int32 AttackIndex = 0;		//? Index of the current attack in the array
	bool bIsAttacking = false;	//? Flag if the character is attacking or not
	bool bSaveCombo = false;	//? Flag to save the combo in the Montages
	TSet<TObjectPtr<AActor>> EnemiesHitThisAttack;	//? Set of Actor Pointers hit this attack, so we don't hit the same Actor multiple times
	UAnimMontage* CurrentAttackMontage;
	
	/*?-------------------|TARGETING | VARS|--------------------*/
	TArray<TObjectPtr<AActor>> TargetActors;
	bool bIsTargeting = false;
	TObjectPtr<AActor> CurrentTargetActor;
	FTimerHandle LerpToTargetActorTimer;
	int32 TargetIndex = 0;
	
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
	
	//TEMP
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HitReactionMontage;

	/*?-------------------|Anims|--------------------*/
	TObjectPtr<class UCharacterAnimInstance> AnimInstance;
	
	/*?-------------------|VFX|--------------------*/
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class UNiagaraSystem> HitVFX;
	
	/*?-------------------|SOUND|--------------------*/
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> AttackSound;
};
