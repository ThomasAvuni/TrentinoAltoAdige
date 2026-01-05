// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TrentinoAltoAdige/Components/CombatSystemComponent.h"
#include "TrentinoAltoAdige/UI/PlayerHUD.h"
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
	
	GetWorldTimerManager().SetTimer(InteractionTimer, this, &APlayerCharacter::CheckForInteraction, InteractionCheckFreq, true);

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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::InternalAttack);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &APlayerCharacter::InternalTarget);
		EnhancedInputComponent->BindAction(NextTargetAction, ETriggerEvent::Started, this, &APlayerCharacter::InternalNextTarget);
		EnhancedInputComponent->BindAction(Parry, ETriggerEvent::Started, this, &APlayerCharacter::InternalStartParry);
		EnhancedInputComponent->BindAction(Parry, ETriggerEvent::Completed, this, &APlayerCharacter::InternalStopParry);
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Started, this, &APlayerCharacter::HandleBackAction);
	}
}

void APlayerCharacter::HandleBackAction()
{
	if (ActiveInteractionSession)
		InternalStopInteract();
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

void APlayerCharacter::InternalStartParry()
{
	bCanSprint = false;
	if (CombatSystemComponent)
		CombatSystemComponent->StartParry();
}

void APlayerCharacter::InternalStopParry()
{
	bCanSprint = true;
	
	if (CombatSystemComponent)
		CombatSystemComponent->EndParry();
}

void APlayerCharacter::ResetPlayerMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetController()->SetIgnoreLookInput(false);
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
}

void APlayerCharacter::Interact()
{
	//(X=25.683607,Y=29.999998,Z=37.950425)
	/*
	* {
	"Tagged": [
		[
			"RelativeLocation",
			"(X=93.657124,Y=109.178393,Z=32.362580)"
		],
		[
			"RelativeRotation",
			"(Pitch=-3.136040,Yaw=-8.103552,Roll=-1.380143)"
		],
		[
			"RelativeScale3D",
			"(X=1.000000,Y=1.000000,Z=1.000000)"
		],
		[
			"Mobility",
			"Movable"
		]
	]
}
	 */
	if (ActiveInteractionSession) return;
	
	if (CurrentInteractable)
	{
		EInteractionType Type = CurrentInteractable->GetInteractionType();
		if (Type == Duration)
		{
			ActiveInteractionSession = CurrentInteractable;	
		}
		
		CurrentInteractable->Interact(this);
	}
}

void APlayerCharacter::InternalStopInteract()
{
	if (ActiveInteractionSession)
	{
		ActiveInteractionSession->StopInteract();
		ActiveInteractionSession = nullptr;
	}
}

void APlayerCharacter::CheckForInteraction()
{
	FVector Start = GetActorLocation() + FVector(0, 0, 88.f);
	FVector End = Start + (GetCamera()->GetForwardVector() * 400.f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params))
	{
		if (IInteractionInterface* Interaction = Cast<IInteractionInterface>(HitResult.GetActor()))
		{
			CurrentInteractable = Interaction;
			CurrentInteractable->ShowInteractionWidget();
		}
		else
		{
			CurrentInteractable = nullptr;
		}
	}
	else
	{
		if (CurrentInteractable)
			CurrentInteractable->HideInteractionWidget();
		CurrentInteractable = nullptr;
	}
} 
