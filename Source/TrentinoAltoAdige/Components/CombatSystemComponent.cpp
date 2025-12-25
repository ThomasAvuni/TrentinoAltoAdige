// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSystemComponent.h"

#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/DebugMacros.h"
#include "TrentinoAltoAdige/Characters/Animation/CharacterAnimInstance.h"
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
	if (!OwnerRef->IsWeaponEquipped()) return;
	
	if (bIsAttacking && !bSaveCombo) return;
	
	if (OwnerRef)
	{
		if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
		{
			if (UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(OwnerRef->GetCharacterMesh()->GetAnimInstance()))
			{
				Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordHandSocket"));
				const TArray<FComboAttack>& ComboAttacks = Weapon->GetWeaponComboAttacks();
				int32 CurrentAttackIndex = AttackIndex % ComboAttacks.Num();
				if (UAnimMontage* AttackMontage = ComboAttacks[CurrentAttackIndex].AttackMontage)
				{
					if (AttackSound)
						UGameplayStatics::PlaySound2D(GetWorld(), AttackSound, 0.65f);
					bIsAttacking = true;
					bSaveCombo = false;
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
	bIsAttacking = false;
	AttackIndex = 0;
	if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
		Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordIdleSocket"));
		
}

void UCombatSystemComponent::PerformTrace()
{
	
}

