// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "InventoryWidget.h"
#include "MainHUD.h"
#include "WeaponUpgradeWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"
#include "TrentinoAltoAdige/Components/DamageComponent.h"
#include "TrentinoAltoAdige/UI/HealthBar.h"
#include "TrentinoAltoAdige/Components/InventoryComponent.h"

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

	if (ClassInventoryWidget)
	{
		if (PlayerRef)
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), ClassInventoryWidget);
			InventoryWidget->InitInventory(PlayerRef->GetInventoryComponent());
			PlayerRef->InitInventoryWidget(InventoryWidget);
		}
	}
	
	if (PlayerRef)
	{
		// HealthBar = CreateWidget<UHealthBar>(GetWorld(), HealthBarClass);
		HealthBar = MainHUD->GetHealthBar();
		if (HealthBar)
		{
			HealthBar->DamageComponent = PlayerRef->GetDamageComponent();
			HealthBar->AddToViewport();
		}
	}
	
	if (PlayerRef)
	{
		PlayerRef->GetCombatSystemComponent()->OnWeaponUpgraded.AddDynamic(this, &APlayerHUD::HandleWeaponLevelUpdate);
		PlayerRef->GetCombatSystemComponent()->OnWeaponFailedUpgrade.AddDynamic(this, &APlayerHUD::HandleWeaponFailedUpgrade);
		PlayerRef->GetDamageComponent()->OnHealthChanged.AddDynamic(this, &APlayerHUD::HandleHealthChanged);
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

void APlayerHUD::HandleHealthChanged(float NewHealth)
{
	if (HealthBar)
	{
		HealthBar->UpdateHealthbar(NewHealth);
	}
	
	if (MainHUD)
	{
		MainHUD->UpdateHealthText(PlayerRef->GetDamageComponent()->GetCurrentHealth(), PlayerRef->GetDamageComponent()->GetMaxHealth());
	}
}

