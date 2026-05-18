// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UDamageComponent;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateHealthbar(float NewHealth);
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UDamageComponent* DamageComponent;
};
