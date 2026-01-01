// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "EnemyBase.generated.h"

UCLASS()
class TRENTINOALTOADIGE_API AEnemyBase : public ACharacter, public ICombatInterface
{
public:
	virtual AWeaponBase* GetWeapon() const override;
	virtual USkeletalMeshComponent* GetCharacterMesh() override {return GetMesh();}
	virtual bool IsWeaponEquipped() const override;
	virtual void EquipWeapon() override;
	virtual void UnEquipWeapon() override;
	virtual void ShowTargetWidget() override { InternalShowTarget(); }
	virtual void HideTargetWidget() override { InternalHideTarget();}
private:
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	UFUNCTION(BlueprintImplementableEvent)
	void InternalShowTarget();
	UFUNCTION(BlueprintImplementableEvent)
	void InternalHideTarget();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
