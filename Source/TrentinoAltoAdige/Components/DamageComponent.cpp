// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageComponent.h"


// Sets default values for this component's properties
UDamageComponent::UDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = MaxHealth;
	
	OnHealthChanged.Broadcast(CurrentHealth);
}

// Called when the game starts
void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UDamageComponent::Heal(float HealAmount)
{
	CurrentHealth += HealAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth); 
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UDamageComponent::TakeDamage(FDamage Damage)
{
	if (IsDead) return;
	
	if (!Damage.ShouldDoDamage) return;
	
	CurrentHealth -= Damage.DamageAmount;
	OnDamageResponse.Broadcast(Damage.HitDirection);
	OnHealthChanged.Broadcast(CurrentHealth);
	
	if (CurrentHealth<=0)
	{
		IsDead = true;

		OnDeath.Broadcast();
	}
}
