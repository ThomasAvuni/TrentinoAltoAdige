// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrentinoAltoAdige/World/Items/ItemBase.h"
#include "WeaponBase.generated.h"

class UItemDataAsset;

USTRUCT(BlueprintType)
struct FAttack
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
	const TArray<FAttack>& GetWeaponComboAttacks() const {return ComboAttacks;}
	const TObjectPtr<UAnimMontage> GetPerfectParryCounterAttack() const {return PerfectParryMontage;}

	const TObjectPtr<USoundBase>& GetWeaponHitSound() const {return HitSound;}
	const TObjectPtr<USoundBase>& GetWeaponBlockSound() const {return HitBlockSound;}

	USkeletalMeshComponent* GetMesh() const {return WeaponMesh;}

	UFUNCTION(BlueprintImplementableEvent)
	USceneComponent* GetLeftHandIKSceneComponent();

	void SetNewLevelMesh(USkeletalMesh* NewMesh);
	
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
	TArray<FAttack> ComboAttacks;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	TObjectPtr<UAnimMontage> ParryMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	TObjectPtr<UAnimMontage> PerfectParryMontage;
	//Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> HitBlockSound;
	
	//Sockets
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName IdleSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName HandSocket;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName BackSocket;
	
};
