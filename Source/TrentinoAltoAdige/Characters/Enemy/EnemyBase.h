// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"
#include "EnemyBase.generated.h"

class UCombatSystemComponent;

UCLASS()
class TRENTINOALTOADIGE_API AEnemyBase : public ACharacter, public ICombatInterface, public IGetComponentInterface
{
	GENERATED_BODY()

public:
	virtual AWeaponBase* GetWeapon() const override;
	virtual UCombatSystemComponent* GetCombatSystemComponent() const override {return CombatSystemComponent;}
	virtual bool IsParrying() override{return CombatSystemComponent->IsParrying();}
	virtual bool IsPerfectParrying() override {return CombatSystemComponent->IsPerfectParrying();}
	virtual void SnapToTarget() override {InternalSnapToTarget();}
	virtual USkeletalMeshComponent* GetCharacterMesh() override {return GetMesh();}
	virtual bool IsWeaponEquipped() const override;
	virtual void EquipWeapon() override;
	virtual void UnEquipWeapon() override;
	virtual void ShowTargetWidget() override { InternalShowTarget(); }
	virtual void HideTargetWidget() override { InternalHideTarget();}
	virtual void HandlePerfectParry() override;
	virtual void HandleParry() override;
	virtual UCombatSystemComponent* GetCombatSystemComponent() override {return CombatSystemComponent;}
	virtual ETeam GetTeam() override {return Enemy;}

	// Sets default values for this character's properties
	AEnemyBase();

	UFUNCTION(BlueprintImplementableEvent)
	void InternalShowTarget();
	UFUNCTION(BlueprintImplementableEvent)
	void InternalHideTarget();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void InternalSnapToTarget();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components | Combat")
	UCombatSystemComponent* CombatSystemComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations | Weapon")
	TObjectPtr<UAnimMontage> EquipFromBackWeapon;
	
};
