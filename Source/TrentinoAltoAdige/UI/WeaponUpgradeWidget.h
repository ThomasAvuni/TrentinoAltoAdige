// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponUpgradeWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UWeaponUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponLevel(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponFailedUpgradeMessage(const FString& Message);
};
