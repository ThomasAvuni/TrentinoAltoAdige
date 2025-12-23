// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"

#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"
#include "TrentinoAltoAdige/Weapons/WeaponBase.h"

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OwnerRef = Cast<IGetComponentInterface>(GetOwningActor());
}

void UCharacterAnimInstance::AnimNotify_EquipWeapon()
{
	if (ICombatInterface* Character = Cast<ICombatInterface>(GetOwningActor()))
	{
		Character->GetWeapon()->AttachToComponent(Character->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordHandSocket"));
	}
}

void UCharacterAnimInstance::AnimNotify_UnEquipWeapon()
{
	if (ICombatInterface* Character = Cast<ICombatInterface>(GetOwningActor()))
	{
		Character->GetWeapon()->AttachToComponent(Character->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordBackSocket"));
	}
}

void UCharacterAnimInstance::AnimNotify_SaveCombo()
{
	if (OwnerRef)
		OwnerRef->GetCombatSystemComponent()->SaveCombo();
}

void UCharacterAnimInstance::AnimNotify_RestCombo()
{
	if (OwnerRef)
		OwnerRef->GetCombatSystemComponent()->ResetCombo();
}
