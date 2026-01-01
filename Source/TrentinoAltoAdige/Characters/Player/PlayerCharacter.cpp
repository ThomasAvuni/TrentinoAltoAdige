// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/Weapons/WeaponBase.h"
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//CombatComp
	CombatSystemComponent = CreateDefaultSubobject<UCombatSystemComponent>("CombatSysComp");
	WeaponHolding = EWeaponHoldingType::None;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	if (WeaponClass)
	{
		//Spawnare nel mondo l'arma, e aggangiarla al personaggio
		FActorSpawnParameters params;
		params.Owner = this;
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, params);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordBackSocket"));
	}

	AnimInstance = GetMesh()->GetAnimInstance();
	
//!!TEMP: SOLO PER DEBUG
#if DEBUG_BUILD && 0
	EquipWeapon();
#endif
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleWeapon);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::InternalAttack);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &APlayerCharacter::InternalTarget);
		EnhancedInputComponent->BindAction(NextTargetAction, ETriggerEvent::Started, this, &APlayerCharacter::InternalNextTarget);
	}
}

#pragma region Weapon

void APlayerCharacter::ToggleWeapon()
{
	//Se stiamo sprintando, non facciamo nulla
	if (bIsSprinting || CombatSystemComponent->IsEquippingWeapon())
		return;
	
	//Se NON si è già equipaggiata l'arma, si equipaggia e esce dalla funzinoe
	if (!CombatSystemComponent->IsWeaponEquipped())
	{
		InternalEquipWeapon();
		return;
	}
	//Se è già equipaggiata si rimette dietro il personaggio
	InternalUnEquipWeapon();
}

void APlayerCharacter::InternalEquipWeapon()
{
	if (CombatSystemComponent)
		CombatSystemComponent->EquipWeapon(CurrentWeapon->GetIdleSocket(), EquipFromBackWeapon);
}

void APlayerCharacter::InternalUnEquipWeapon()
{
	if (CombatSystemComponent)
		CombatSystemComponent->UnEquipWeapon(CurrentWeapon->GetHandSocket(), UnEquipFromHandWeapon);
}

void APlayerCharacter::InternalAttack()
{
	CombatSystemComponent->Attack();
}
#pragma endregion

void APlayerCharacter::InternalTarget()
{
	if (CombatSystemComponent)
		CombatSystemComponent->Target();
}

void APlayerCharacter::InternalNextTarget()
{
	if (CombatSystemComponent)
		CombatSystemComponent->NextTarget();
}
