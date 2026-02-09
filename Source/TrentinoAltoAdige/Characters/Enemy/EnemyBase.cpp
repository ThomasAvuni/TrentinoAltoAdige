// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/CapsuleComponent.h"
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
	DamageComponent->OnDamageResponse.AddDynamic(this, &AEnemyBase::OnDamageResponse);
}

void AEnemyBase::OnDeath()
{
	bCanBeTargeted = false;
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FVector BackwardDirection = -GetActorForwardVector();
	GetMesh()->AddImpulse(BackwardDirection * 3500.f, NAME_None, true);
	FTimerHandle T;
	GetWorldTimerManager().SetTimer(T, [this]
	{
		if (CurrentWeapon)
			CurrentWeapon->Destroy();
		Destroy();
	}, 5.f, false);
}

void AEnemyBase::OnDamageResponse(EHitDirection HitDirection)
{
	switch (HitDirection) {
	case HitNone:
		break;
	case Front:
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (FrontHitMontage)
				AnimInstance->Montage_Play(FrontHitMontage);
		}
		break;
	case Back:
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (BackHitMontage)
				AnimInstance->Montage_Play(BackHitMontage);
		}
		break;
	case Side:
		break;
	}
}
