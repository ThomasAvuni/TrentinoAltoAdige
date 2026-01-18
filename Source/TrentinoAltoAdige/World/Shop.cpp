// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"
#include "TrentinoAltoAdige/UI/MainHUD.h"
#include "TrentinoAltoAdige/UI/PlayerHUD.h"
#include "TrentinoAltoAdige/UI/WeaponUpgradeWidget.h"


// Sets default values
AShop::AShop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShop::Interact(AActor* Interactor)
{
	Super::Interact(Interactor);

	if (APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (APawn* ControlledPawn = Controller->GetPawn())
		{
			// Resetta le velocità e le accelerazioni residue
			ControlledPawn->ConsumeMovementInputVector();
			
			if (APlayerCharacter* Character = Cast<APlayerCharacter>(ControlledPawn))
			{
				Character->GetCharacterMovement()->StopMovementImmediately();
				Character->GetCharacterMovement()->DisableMovement();
				Character->DisableInput(Controller);
				Character->GetController()->SetIgnoreLookInput(true);
				Character->StartShopCameraAnimation();
				Character->bIsInShop = true;
			}
		}

		if (APlayerHUD* HUD = Cast<APlayerHUD>(Controller->GetHUD()))
		{
			if (UMainHUD* MainHUD = HUD->GetMainHUD())
			{
				MainHUD->PushMenu(HUD->GetWeaponUpgradeWidgetClass());
			}
		}
	}
}

void AShop::StopInteract()
{
	Super::StopInteract();
	
	if (APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (APawn* ControlledPawn = Controller->GetPawn())
		{
			if (APlayerCharacter* Character = Cast<APlayerCharacter>(ControlledPawn))
			{
				Character->StopShopCameraAnimation();
				Character->bIsInShop = false;
			}
		}
		
		if (APlayerHUD* HUD = Cast<APlayerHUD>(Controller->GetHUD()))
		{
			Controller->bShowMouseCursor = false;
			
			if (UMainHUD* MainHUD = HUD->GetMainHUD())
			{
				if (UCommonActivatableWidget* ActiveWidget = MainHUD->GetMenuStack()->GetActiveWidget())
				{
					ActiveWidget->DeactivateWidget();
				}
			}
		}
	}
}

