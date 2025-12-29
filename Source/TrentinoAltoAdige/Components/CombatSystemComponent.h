// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatSystemComponent.generated.h"


class AEnemyBase;
class ICombatInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRENTINOALTOADIGE_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatSystemComponent();

	//Attacks
	UFUNCTION(BlueprintPure)
	bool IsAttacking() const {return bIsAttacking;}
	void SaveCombo();
	void ResetCombo();
	void Attack();
	void PerformTrace();
	void ResetEnemiesHitThisAttack() {EnemiesHitThisAttack.Empty();}

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
	ICombatInterface* OwnerRef;	//? Combat Owner Ref
	TObjectPtr<class APlayerCharacter> PlayerOwnerRef;
	int32 AttackIndex = 0;		//? Index of the current attack in the array
	bool bIsAttacking = false;	//? Flag if the character is attacking or not
	bool bSaveCombo = false;	//? Flag to save the combo in the Montages
	TSet<TObjectPtr<AActor>> EnemiesHitThisAttack;	//? Set of Actor Pointers hit this attack, so we don't hit the same Actor multiple times

	void ApplyHitStop(AActor* Actor, float Duration, float TimeDilation) const;

	//TEMP
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HitReactionMontage;
	
	//VFX
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class UNiagaraSystem> HitVFX;
	
	//Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> AttackSound;
};
