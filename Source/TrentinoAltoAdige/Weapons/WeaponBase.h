// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

USTRUCT(BlueprintType)
struct FComboAttack
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animation")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animation")
	UAnimMontage* EnemyReactionMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float DamageMultiplier = 0.f;
};

UCLASS()
class TRENTINOALTOADIGE_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponBase();

	UFUNCTION(BlueprintCallable)
	const float& GetWeaponBaseDamage() const {return WeaponBaseDamage;}
	
	UFUNCTION(BlueprintCallable)
	const TArray<FComboAttack>& GetWeaponComboAttacks() const {return ComboAttacks;}

	const TObjectPtr<USoundBase>& GetWeaponHitSound() const {return AttackSound;}

	USkeletalMeshComponent* GetMesh() const {return WeaponMesh;}
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float WeaponBaseDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	TArray<FComboAttack> ComboAttacks;
	
	//Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> AttackSound;
};
