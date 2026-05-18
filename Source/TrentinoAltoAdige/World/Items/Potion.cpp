// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"

#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"
#include "TrentinoAltoAdige/Components/DamageComponent.h"
#include "TrentinoAltoAdige/Components/InventoryComponent.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"


class APlayerCharacter;
// Sets default values
APotion::APotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APotion::Interact(AActor* Interactor)
{
	if (APlayerCharacter *Player = Cast<APlayerCharacter>(Interactor))
	{
		FInventoryItem Item;
		Item.ItemDescription = GetItemDescription();
		Item.Quantity = 1;
		Player->GetInventoryComponent()->AddItem(Item);
	}
}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
	
}

void APotion::UseItem(AActor* User)
{
	
	if (ICombatInterface* Char = Cast<ICombatInterface>(User))
	{
		if (UDamageComponent* DamageComponent = Char->GetDamageComponent())
		{
			DamageComponent->Heal(HealthGiven);
		}
	}
}
