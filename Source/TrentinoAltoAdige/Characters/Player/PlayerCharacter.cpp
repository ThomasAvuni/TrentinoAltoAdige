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
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleWeapon);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
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
		InternalEquipWeapon();
		return;
	}
	//Se è già equipaggiata si rimette dietro il personaggio
	InternalUnEquipWeapon();
}

void APlayerCharacter::InternalEquipWeapon()
{
	// Verifica che sia l'istanza di animazione che l'asset del montage siano validi
	if (AnimInstance && EquipFromBackWeapon)
	{
		// Attiva il flag di occupazione per impedire altre azioni durante l'animazione
		bIsEquippingWeapon = true;
		// Avvia la riproduzione dell'animazione di equipaggiamento dalla schiena
		AnimInstance->Montage_Play(EquipFromBackWeapon);
		// Dichiarazione del delegate per intercettare la fine del montage
		FOnMontageEnded OnMontageEnded;
		// Definizione della logica da eseguire al termine dell'animazione tramite Lambda
		// Viene catturato 'this' per poter accedere alle variabili della classe
		OnMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
		   // L'animazione è terminata o interrotta: sblocca lo stato del personaggio
		   bIsEquippingWeapon = false;
			// Aggiorna lo stato logico: l'arma è ora considerata equipaggiata
			bIsWeaponEquipped = true;
			WeaponHolding = EWeaponHoldingType::GreatSword;
		});
		// Associa formalmente il delegate al montage specifico appena avviato
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, EquipFromBackWeapon);
	}
}

void APlayerCharacter::InternalUnEquipWeapon()
{
	//Verifica che l'asset del Montage (l'animazione) sia valido prima di procedere
	if (AnimInstance && UnEquipFromHandWeapon)
	{
		//Avvia la riproduzione del Montage sull'istanza di animazione corrente
		AnimInstance->Montage_Play(UnEquipFromHandWeapon);
		//Imposta un flag di stato per bloccare altre azioni (es. sparare) durante l'animazione
		bIsEquippingWeapon = true;
		bIsWeaponEquipped = false;
		//Dichiarazione di un "Delegate" per gestire l'evento di fine animazione
		FOnMontageEnded OnMontageEnded;
		//Lega una funzione Lambda al delegate. 
		// Questa funzione verrà eseguita AUTOMATICAMENTE quando il montage finisce o viene interrotto.
		OnMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
		   //Resetta il flag: l'azione è terminata, il personaggio può fare altro
		   bIsEquippingWeapon = false;
		   //Notifica ad altri sistemi (es. UI o Inventory) che l'arma è stata riposta
		   OnWeaponUnEquipped.Broadcast();
			
			WeaponHolding = EWeaponHoldingType::None;
		});

		//Registra il delegate appena creato specificamente per questo Montage
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, UnEquipFromHandWeapon);
	}
}

void APlayerCharacter::Attack()
{
	CombatSystemComponent->Attack();
}
#pragma endregion
