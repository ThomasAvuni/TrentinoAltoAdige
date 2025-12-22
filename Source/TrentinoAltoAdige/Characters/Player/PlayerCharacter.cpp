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
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleWeapon);
	}
}

#pragma region Weapon
void APlayerCharacter::ToggleWeapon()
{
	//Se stiamo sprintando, non facciamo nulla
	if (bIsSprinting || bIsEquippingWeapon)
		return;
	
	//Se NON si è già equipaggiata l'arma, si equipaggia e esce dalla funzinoe
	if (!bIsWeaponEquipped)
	{
		EquipWeapon();
		return;
	}
	//Se è già equipaggiata si rimette dietro il personaggio
	UnEquipWeapon();
}

void APlayerCharacter::EquipWeapon()
{
		if (UnEquipFromHandWeapon)
		{
			bIsWeaponEquipped = true;
			bIsEquippingWeapon = true;
			AnimInstance->Montage_Play(EquipFromBackWeapon);
			FOnMontageEnded OnMontageEnded;
			OnMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			{
				bIsEquippingWeapon = false;
			});
			AnimInstance->Montage_SetEndDelegate(OnMontageEnded, EquipFromBackWeapon);
		}
}

void APlayerCharacter::UnEquipWeapon()
{
	if (EquipFromBackWeapon)
	{
		AnimInstance->Montage_Play(UnEquipFromHandWeapon);
		bIsEquippingWeapon = true;
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			bIsEquippingWeapon = false;
			OnWeaponUnEquipped.Broadcast();
		});
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, UnEquipFromHandWeapon);
	}
}
#pragma endregion
