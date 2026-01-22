// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"

#include "../DebugMacros.h"


// Sets default values for this component's properties
UDamageComponent::UDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = MaxHealth;
}

void UDamageComponent::Heal(float HealAmount)
{
	CurrentHealth += HealAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth); 
}


void UDamageComponent::TakeDamage(FDamage Damage)
{
	if (IsDead) return;
	if (!Damage.ShouldDoDamage) return;
	CurrentHealth -= Damage.DamageAmount;
	//Broadcast del delegate OnHealthChanged
	DBG_LINE("{}", CurrentHealth);

	if (CurrentHealth<=0)
	{
		IsDead = true;

		OnDeath.Broadcast();
	}
}

// Called when the game starts
void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


