// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"

#include "TrentinoAltoAdige/Components/DamageComponent.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"


// Sets default values
APotion::APotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
	
}

void APotion::UseItem(AActor* User)
{
	Super::UseItem(User);
	
	if (ICombatInterface* Char = Cast<ICombatInterface>(User))
	{
		if (UDamageComponent* DamageComponent = Char->GetDamageComponent())
		{
			DamageComponent->Heal(HealthGiven);
		}
	}
}
