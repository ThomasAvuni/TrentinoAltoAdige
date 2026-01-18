// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "MainHUD.h"
#include "WeaponUpgradeWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (MainHUDClass)
	{
		MainHUD = CreateWidget<UMainHUD>(GetWorld(), MainHUDClass);
		if (MainHUD)
		{
			MainHUD->AddToViewport();
		}
	}
	
	if (PlayerRef)
	{
		PlayerRef->GetCombatSystemComponent()->OnWeaponUpgraded.AddDynamic(this, &APlayerHUD::HandleWeaponLevelUpdate);
		PlayerRef->GetCombatSystemComponent()->OnWeaponFailedUpgrade.AddDynamic(this, &APlayerHUD::HandleWeaponFailedUpgrade);
	}
}

void APlayerHUD::HandleWeaponLevelUpdate(int32 NewLevel)
{
	if (MainHUD && MainHUD->GetMenuStack())
	{
		if (UWeaponUpgradeWidget* Widget = Cast<UWeaponUpgradeWidget>(MainHUD->GetMenuStack()->GetActiveWidget()))
		{
			Widget->UpdateWeaponLevel(NewLevel);
		}
	}
}

void APlayerHUD::HandleWeaponFailedUpgrade(FString Message)
{
	if (MainHUD && MainHUD->GetMenuStack())
	{
		if (UWeaponUpgradeWidget* Widget = Cast<UWeaponUpgradeWidget>(MainHUD->GetMenuStack()->GetActiveWidget()))
		{
			Widget->UpdateWeaponFailedUpgradeMessage(Message);
		}
	}
}
