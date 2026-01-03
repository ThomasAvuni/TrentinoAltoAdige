// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRENTINOALTOADIGE_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual class AWeaponBase* GetWeapon() const = 0;
	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetCharacterMesh() = 0;
	UFUNCTION(BlueprintCallable)
	virtual bool IsWeaponEquipped() const = 0;
	virtual void EquipWeapon() = 0;
	virtual void UnEquipWeapon() = 0;
	virtual void ShowTargetWidget(){}
	virtual void HideTargetWidget(){}
	virtual void SetMovementToWalk(){}
	virtual void ResetMovement(){}
};
