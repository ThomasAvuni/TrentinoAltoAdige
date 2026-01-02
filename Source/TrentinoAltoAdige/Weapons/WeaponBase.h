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
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float DamageMultiplier = 0.f;
};

UCLASS()
class TRENTINOALTOADIGE_API AWeaponBase : public AActor
{

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponBase();

	UFUNCTION(BlueprintCallable)
	const float& GetWeaponBaseDamage() const {return WeaponBaseDamage;}
	
	UFUNCTION(BlueprintCallable)
	const TArray<FComboAttack>& GetWeaponComboAttacks() const {return ComboAttacks;}

	const TObjectPtr<USoundBase>& GetWeaponHitSound() const {return HitSound;}

	USkeletalMeshComponent* GetMesh() const {return WeaponMesh;}
	
	const FName& GetIdleSocket() const { return IdleSocket; }
	const FName& GetHandSocket() const { return HandSocket; }
	const FName& GetBackSocket() const { return BackSocket; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float WeaponBaseDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	TArray<FComboAttack> ComboAttacks;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	TObjectPtr<UAnimMontage> ParryMontage;
	//Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
	
	//Sockets
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName IdleSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName HandSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName BackSocket;
};
