// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Components/DamageComponent.h"
#include "TrentinoAltoAdige/Weapons/WeaponBase.h"


AWeaponBase* AEnemyBase::GetWeapon() const
{
	return CurrentWeapon;
}

bool AEnemyBase::IsWeaponEquipped() const
{
	return CombatSystemComponent->IsWeaponEquipped();
}

//! Spostare in CombatSystemComponent
void AEnemyBase::EquipWeapon()
{
}

void AEnemyBase::UnEquipWeapon()
{
}

void AEnemyBase::HandlePerfectParry()
{
}

void AEnemyBase::HandleParry()
{
}

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CombatSystemComponent = CreateDefaultSubobject<UCombatSystemComponent>("CombatSysComp");

	DamageComponent = CreateDefaultSubobject<UDamageComponent>("DamageComponent");
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters params;
	params.Owner = this;
	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, params);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordBackSocket"));
	CombatSystemComponent->EquipWeapon(CurrentWeapon->GetIdleSocket(), EquipFromBackWeapon);

	DamageComponent->OnDeath.AddDynamic(this, &AEnemyBase::OnDeath);
	
}

void AEnemyBase::OnDeath()
{
	Destroy();
}
