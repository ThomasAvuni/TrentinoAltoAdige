// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class APlayerCharacter;
class UWeaponUpgradeWidget;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWeaponUpgradeWidget> WeaponUpgradeWidgetClass;
	TObjectPtr<UWeaponUpgradeWidget> WeaponUpgradeWidget;
	
private:
	TObjectPtr<APlayerCharacter> PlayerRef;
	UFUNCTION()
	void HandleWeaponLevelUpdate(int32 NewLevel);
	
	UFUNCTION()
	void HandleWeaponFailedUpgrade(FString Message);
};
