// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "WeaponUpgradeWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (WeaponUpgradeWidgetClass)
	{
		WeaponUpgradeWidget = CreateWidget<UWeaponUpgradeWidget>(GetWorld(), WeaponUpgradeWidgetClass);
		if (WeaponUpgradeWidget)
		{
			WeaponUpgradeWidget->AddToViewport();
			WeaponUpgradeWidget->SetVisibility(ESlateVisibility::Collapsed);
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
	if (WeaponUpgradeWidget)
	{
		WeaponUpgradeWidget->UpdateWeaponLevel(NewLevel);
	}
}

void APlayerHUD::HandleWeaponFailedUpgrade(FString Message)
{
	if (WeaponUpgradeWidget)
	{
		WeaponUpgradeWidget->UpdateWeaponFailedUpgradeMessage(Message);
	}
}

void APlayerHUD::ShowUpgradeWeaponWidget()
{
	if (WeaponUpgradeWidget)
	{
		WeaponUpgradeWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APlayerHUD::HideUpgradeWeaponWidget()
{
	if (WeaponUpgradeWidget)
	{
		WeaponUpgradeWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
