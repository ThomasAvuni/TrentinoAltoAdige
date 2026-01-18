// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "MainHUD.generated.h"

class UCommonActivatableWidget;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UMainHUD : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void PushMenu(TSubclassOf<UCommonActivatableWidget> MenuClass);
	UFUNCTION(BlueprintCallable)
	UCommonActivatableWidgetStack* GetMenuStack() {return MenuStack;}
protected:
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* MenuStack;
};
