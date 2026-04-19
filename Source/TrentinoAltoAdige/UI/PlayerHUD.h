// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UInventoryWidget;
class UMainHUD;
class APlayerCharacter;
class UWeaponUpgradeWidget;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	TObjectPtr<UWeaponUpgradeWidget> GetWeaponUpgradeWidget() const { return WeaponUpgradeWidget; }
	TSubclassOf<UWeaponUpgradeWidget> GetWeaponUpgradeWidgetClass() const { return WeaponUpgradeWidgetClass; }
	TObjectPtr<UMainHUD> GetMainHUD() const { return MainHUD; }
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWeaponUpgradeWidget> WeaponUpgradeWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainHUD> MainHUDClass;
	UPROPERTY()
	TObjectPtr<UWeaponUpgradeWidget> WeaponUpgradeWidget;
	UPROPERTY()
	TObjectPtr<UMainHUD> MainHUD;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

private:
	TObjectPtr<APlayerCharacter> PlayerRef;
	TObjectPtr<UInventoryWidget> InventoryWidget;
	UFUNCTION()
	void HandleWeaponLevelUpdate(int32 NewLevel);
	
	UFUNCTION()
	void HandleWeaponFailedUpgrade(FString Message);
	
	UFUNCTION()
	
};
