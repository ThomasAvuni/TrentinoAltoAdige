// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystemComponent.h"

#include "TrentinoAltoAdige/Characters/CharacterAnimInstance.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Weapons/WeaponBase.h"


// Sets default values for this component's properties
UCombatSystemComponent::UCombatSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	OwnerRef = Cast<ICombatInterface>(GetOwner());
}


// Called when the game starts
void UCombatSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatSystemComponent::Attack()
{
	if (bIsAttacking) return;
	
	if (OwnerRef)
	{
		if (const AWeaponBase* Weapon = OwnerRef->GetWeapon())
		{
			if (UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(OwnerRef->GetCharacterMesh()->GetAnimInstance()))
			{
				const TArray<FComboAttack>& ComboAttacks = Weapon->GetWeaponComboAttacks();
				int32 CurrentAttackIndex = AttackIndex % ComboAttacks.Num();
				if (UAnimMontage* AttackMontage = ComboAttacks[CurrentAttackIndex].AttackMontage)
				{
					bIsAttacking = true;
					AnimInstance->Montage_Play(AttackMontage);
					AttackIndex++;
				}
			}
		}
	}
}

void UCombatSystemComponent::SaveCombo()
{
	if (bIsAttacking)
		bSaveCombo = true;
}

void UCombatSystemComponent::ResetCombo()
{
	bSaveCombo = false;
	AttackIndex = 0;
}
