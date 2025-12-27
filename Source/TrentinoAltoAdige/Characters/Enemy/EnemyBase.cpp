// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "TrentinoAltoAdige/DebugMacros.h"


AWeaponBase* AEnemyBase::GetWeapon() const
{
	return nullptr;
}

USkeletalMeshComponent* AEnemyBase::GetCharacterMesh() const
{
	return GetMesh();
}

bool AEnemyBase::IsWeaponEquipped() const
{
	return false;
}

void AEnemyBase::EquipWeapon()
{
}

void AEnemyBase::UnEquipWeapon()
{
}

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyBase::DBG_TakeDamage()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString("HIT"));
}


