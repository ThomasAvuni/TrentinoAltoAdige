// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UTargetWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayPopupAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void StopPopupAnimation();
};
