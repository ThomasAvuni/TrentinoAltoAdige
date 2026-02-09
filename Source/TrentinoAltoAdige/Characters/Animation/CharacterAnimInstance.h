// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

class IGetComponentInterface;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void AnimNotify_EquipWeapon();
	UFUNCTION()
	void AnimNotify_UnEquipWeapon();
	UFUNCTION()
	void AnimNotify_SaveAttack();
	UFUNCTION()
	void AnimNotify_ResetCombo();
	UFUNCTION()
	void AnimNotify_SnapToTarget();
	UFUNCTION()
	void AnimNotify_DisableMovement();
	UFUNCTION()
	void AnimNotify_ResetActorsThisAttack();
protected:
	virtual void NativeBeginPlay() override;
	IGetComponentInterface* OwnerRef;
};
