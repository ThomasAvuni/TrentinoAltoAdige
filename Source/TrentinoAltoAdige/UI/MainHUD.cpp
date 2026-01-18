// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"

#include "CommonActivatableWidget.h"

void UMainHUD::PushMenu(TSubclassOf<UCommonActivatableWidget> MenuClass)
{
	if (MenuStack && MenuClass)
	{
		MenuStack->AddWidget(MenuClass);
	}
}
