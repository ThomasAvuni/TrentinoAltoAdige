// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUpgradeWidget.h"

#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"

void UWeaponUpgradeWidget::UpgradeWeapon()
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		//Chiamata a inventorycomponent in futuro
		Player->GetCombatSystemComponent()->UpgradeWeapon(12);
	}
}
