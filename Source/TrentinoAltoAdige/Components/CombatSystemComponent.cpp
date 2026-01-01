// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatSystemComponent.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TrentinoAltoAdige/DebugMacros.h"
#include "TrentinoAltoAdige/Characters/Animation/CharacterAnimInstance.h"
#include "TrentinoAltoAdige/Characters/Player/PlayerCharacter.h"
#include "TrentinoAltoAdige/Interfaces/CombatInterface.h"
#include "TrentinoAltoAdige/Weapons/WeaponBase.h"


// Sets default values for this component's properties
UCombatSystemComponent::UCombatSystemComponent()
{
	// Questo componente non necessita del tick ogni frame, quindi disabilitato.
	PrimaryComponentTick.bCanEverTick = false;
	
}

// Called when the game starts
void UCombatSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	// Ottengo riferimenti all'interfaccia di combattimento e al player owner (se presenti).
	// Cast su GetOwner() può restituire nullptr in fase di costruzione; tenere presente.
	OwnerRef = GetOwner<ICombatInterface>();
	PlayerOwnerRef = GetOwner<APlayerCharacter>();
	AnimInstance = Cast<UCharacterAnimInstance>(OwnerRef->GetCharacterMesh()->GetAnimInstance());
}

#pragma region Attack
void UCombatSystemComponent::Attack()
{
	// Controllo rapido: se non è equipaggiata un'arma non procedere.
	if (!OwnerRef->IsWeaponEquipped()) return;

	// Se sto già attaccando e non è stato permesso il salvataggio combo, esco.
	if (bIsAttacking && !bSaveCombo) return;

	// Controllo che OwnerRef sia valido (null-check) e recupero l'arma.
	if (OwnerRef)
	{
		if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
		{
			// Ottengo l'anim instance della mesh del personaggio (per riprodurre montage).
			if (AnimInstance)
			{
				// Assicuro che l'arma sia attaccata alla socket della mano per l'attacco.
				Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordHandSocket"));

				// Ottengo gli attacchi a combo definiti dall'arma.
				const TArray<FComboAttack>& ComboAttacks = Weapon->GetWeaponComboAttacks();
				int32 CurrentAttackIndex = AttackIndex % ComboAttacks.Num();

				// Se il montage dell'attacco esiste, lo eseguo.
				if (UAnimMontage* AttackMontage = ComboAttacks[CurrentAttackIndex].AttackMontage)
				{
					// Esegui suono d'attacco se presente.
					if (AttackSound)
						UGameplayStatics::PlaySound2D(GetWorld(), AttackSound, 0.65f);

					// Stato: sto attaccando, reset del flag save combo.
					bIsAttacking = true;
					bSaveCombo = false;

					// Se è un player, applica effetto camera lerp durante l'attacco.
					if (PlayerOwnerRef)
						PlayerOwnerRef->LerpCamToAttackPosition();
					
					// Avvio il montage di attacco.
					AnimInstance->Montage_Play(AttackMontage);

					// Incremento l'indice degli attacchi per il prossimo colpo di combo.
					AttackIndex++;
				}
			}
		}
	}
}

void UCombatSystemComponent::SaveCombo()
{
	// Se attualmente in attacco, permetto di salvare la combo per concatenare l'attacco successivo.
	if (bIsAttacking)
		bSaveCombo = true;
}

void UCombatSystemComponent::ResetCombo()
{
	// Ripristino la camera del player se necessario.
	if (PlayerOwnerRef)
		PlayerOwnerRef->ResetCam();
	
	// Reset degli stati e dei riferimenti relativi alla combo.
	bSaveCombo = false;
	bIsAttacking = false;
	CurrentHitActor = nullptr;
	AttackIndex = 0;

	// Riattacco l'arma alla socket di idle (posizione a riposo).
	if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
		Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SwordIdleSocket"));
}

void UCombatSystemComponent::PerformTrace()
{
	// Recupero arma e definisco start/end per lo sweep basato sui socket della mesh dell'arma.
	if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
	{
		FVector TraceStart = Weapon->GetMesh()->GetSocketLocation(FName("SwordBase"));
		FVector TraceEnd = Weapon->GetMesh()->GetSocketLocation(FName("SwordTip"));
		float fRadius = 20.f; // raggio della sfera usata per lo sweep
		FHitResult HitResult;

		// Solo oggetti di tipo Pawn (es. nemici)
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		// Ignoro l'attore proprietario per evitare auto-hits
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		// Sweep sphere tra TraceStart e TraceEnd
		if (GetWorld()->SweepSingleByObjectType(HitResult, TraceStart, TraceEnd, FQuat::Identity, CollisionObjectQueryParams,
			FCollisionShape::MakeSphere(fRadius), QueryParams))
		{
			AActor* HitActor = HitResult.GetActor();
			// Evito di applicare più volte l'hit allo stesso attore nello stesso attacco
			if (!EnemiesHitThisAttack.Contains(HitActor))
			{
				// Uso l'interfaccia di combattimento per applicare reazioni/danni
				if (ICombatInterface* Enemy = Cast<ICombatInterface>(HitActor))
				{
					// Registro l'attore colpito per evitare clash multipli
					EnemiesHitThisAttack.Add(HitActor);
					// Se è il player, applico camera shake
					if (PlayerOwnerRef)
						PlayerOwnerRef->CameraShake();
					// Spawn VFX se presente
					if (HitVFX)
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFX, HitResult.Location);
					// HitSound se presente
					if (USoundBase* HitSound = Weapon->GetWeaponHitSound())
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
					
					// Calcolo direzione del colpo rispetto alla forward vector del bersaglio
					FVector HitActorForwardVector = HitActor->GetActorForwardVector();
					FVector HitDirection = HitActor->GetActorLocation() - GetOwner()->GetActorLocation();
					HitDirection.Normalize();
					float DotProduct = FVector::DotProduct(HitActorForwardVector, HitDirection);

					if (DotProduct > 0.6f)
					{
					}
					else if (DotProduct < -0.6f)
					{
						Enemy->GetCharacterMesh()->GetAnimInstance()->Montage_Play(HitReactionMontage);
					}
					else
					{
					}
					
					// Memorizzo l'attore corrente colpito
					CurrentHitActor = bIsTargeting ? CurrentTargetActor.Get() : HitActor;
					// Applico hit stop temporale per effetto impatto
					ApplyHitStop(GetOwner(), HitStopDuration, HitStopTimeDilation);
				}
			}
		}
	}
}

void UCombatSystemComponent::ApplyHitStop(AActor* Actor, float Duration, float TimeDilation) const
{
	// Applico dilatazione temporale all'attore per il freeze-frame (hit stop)
	Actor->CustomTimeDilation = TimeDilation;

	// Uso un timer per ripristinare la time dilation dopo "Duration" secondi
	FTimerHandle t;
	GetWorld()->GetTimerManager().SetTimer(t, [this]
	{
		// Ripristino della time dilation al valore di default (1.0)
		GetOwner()->CustomTimeDilation = 1.f;
	}, Duration, false);
}
#pragma endregion

#pragma region Target
void UCombatSystemComponent::Target()
{
	if (!bIsTargeting)
	{
		StartTarget();
		return;
	}
	
	StopTarget();
}

void UCombatSystemComponent::StartTarget()
{
	bIsTargeting = true;
	FVector Start = GetOwner()->GetActorLocation() + FVector(-100.f, 0, 20.f);
	FVector End = PlayerOwnerRef ? Start + (PlayerOwnerRef->GetCamera()->GetForwardVector() * 1200.f)
								 : Start + (GetOwner()->GetActorForwardVector() * 1200.f);  
	float CapsuleRadius = 650.f;
	FVector Dir = End - Start;
	float Distance = Dir.Size();
#if DEBUG_BUILD && 0
	// debug draw: sphere su start/end, linea e capsule lungo la direzione
	DrawDebugSphere(GetWorld(), Start, 8.f, 12, FColor::Yellow, false, 2.f);
	DrawDebugSphere(GetWorld(), End, 8.f, 12, FColor::Blue, false, 2.f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 2.f);


	if (Distance > KINDA_SMALL_NUMBER)
	{
		FVector DirNormal = Dir / Distance;
		FVector Center = Start + Dir * 0.5f;
		float HalfHeight = FMath::Max(10.f, Distance * 0.5f);
		FQuat CapsuleQuat = FRotationMatrix::MakeFromZ(DirNormal).ToQuat();
		DrawDebugCapsule(GetWorld(), Center, HalfHeight, CapsuleRadius, CapsuleQuat, FColor::Red, false, 2.f, 0, 1.f);
	}
#endif
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	if (GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, QueryParams,
		FCollisionShape::MakeCapsule(CapsuleRadius, FMath::Max(10.f, Distance * 0.5f)), CollisionQueryParams))
	{
		float ClosestDist = BIG_NUMBER;
		AActor* ClosestActor = nullptr;
		float ClosestInCone = BIG_NUMBER;
		AActor* BestInCone = nullptr;
		for (FHitResult& HitResult : HitResults)
		{
			AActor* Actor = HitResult.GetActor();
			if (!TargetActors.Contains(Actor))
			{
				TargetActors.AddUnique(Actor);
				
				FVector Direction = Actor->GetActorLocation() - GetOwner()->GetActorLocation();
				Direction.Normalize();
				FVector FacingVector = PlayerOwnerRef ? PlayerOwnerRef->GetCamera()->GetForwardVector() : GetOwner()->GetActorForwardVector();
				float DotProduct = FVector::DotProduct(Direction, FacingVector);
				float Angle = FMath::Cos(FMath::DegreesToRadians(35.f));
				float Dist = FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
				if (DotProduct < Angle)
				{
					if (Dist < ClosestDist)
					{
						ClosestDist = Dist;
						ClosestActor = Actor;
					}
					continue;
				}

				if (Dist < ClosestInCone)
				{
					ClosestInCone = Dist;
					BestInCone = Actor;
				}
			}
		}
		CurrentTargetActor = BestInCone ? BestInCone : ClosestActor;
	}

	if (CurrentTargetActor)
	{
		ACharacter* Char = GetOwner<ACharacter>();
		Char->GetController()->SetIgnoreLookInput(true);
		GetWorld()->GetTimerManager().SetTimer(LerpToTargetActorTimer, [this, Char]
		{
			if (Char)
			{
				FVector TargetLocation = CurrentTargetActor->GetActorLocation();
				if (!bIsAttacking)
					TargetLocation.Z -= 20.f;
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);
				FRotator InterpRot = FMath::RInterpTo(Char->GetControlRotation(), LookAt, GetWorld()->GetDeltaSeconds(), 10.f);
				Char->GetController()->SetControlRotation(InterpRot);
			}
		}, 0.01f, true);
	}
}

void UCombatSystemComponent::NextTarget()
{
	if (bIsTargeting)
	{
		int32 NewIndex = ++TargetIndex % TargetActors.Num();
		CurrentTargetActor = TargetActors[NewIndex];
	}
}

void UCombatSystemComponent::StopTarget()
{
	bIsTargeting = false;
	TargetActors.Empty();
	CurrentTargetActor = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(LerpToTargetActorTimer);
	ACharacter* Char = GetOwner<ACharacter>();
	Char->GetController()->SetIgnoreLookInput(false);
}
#pragma endregion

#pragma region Equipping
void UCombatSystemComponent::EquipWeapon(FName InSocket, UAnimMontage* EquipMontage)
{
	// Verifica che sia l'istanza di animazione che l'asset del montage siano validi
	if (AnimInstance && EquipMontage)
	{
		// Attiva il flag di occupazione per impedire altre azioni durante l'animazione
		bIsEquippingWeapon = true;
		// Avvia la riproduzione dell'animazione di equipaggiamento dalla schiena
		AnimInstance->Montage_Play(EquipMontage);
		// Dichiarazione del delegate per intercettare la fine del montage
		FOnMontageEnded OnMontageEnded;
		// Definizione della logica da eseguire al termine dell'animazione tramite Lambda
		// Viene catturato 'this' per poter accedere alle variabili della classe
		OnMontageEnded.BindLambda([this, InSocket](UAnimMontage* Montage, bool bInterrupted)
		{
			if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
			{
				Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::KeepWorldTransform, InSocket);
				InterpolateWeaponToSocket(Weapon, InSocket);
				// L'animazione è terminata o interrotta: sblocca lo stato del personaggio
				bIsEquippingWeapon = false;
				// Aggiorna lo stato logico: l'arma è ora considerata equipaggiata
				bIsWeaponEquipped = true;
			}
		});
		// Associa formalmente il delegate al montage specifico appena avviato
		AnimInstance->Montage_SetEndDelegate(OnMontageEnded, EquipMontage);
	}
}

void UCombatSystemComponent::UnEquipWeapon(FName InSocket, UAnimMontage* UnEquipMontage)
{
	if (bIsAttacking) return;
	
	if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
	{
		//Verifica che l'asset del Montage (l'animazione) sia valido prima di procedere
		if (AnimInstance && UnEquipMontage)
		{
			Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocket);
			//Avvia la riproduzione del Montage sull'istanza di animazione corrente
			AnimInstance->Montage_Play(UnEquipMontage);
			//Imposta un flag di stato per bloccare altre azioni (es. Sparare) durante l'animazione
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
			});
			//Registra il delegate appena creato specificamente per questo Montage
			AnimInstance->Montage_SetEndDelegate(OnMontageEnded, UnEquipMontage);
		}
	}
}

void UCombatSystemComponent::InterpolateWeaponToSocket(AWeaponBase* Weapon, FName Socket)
{
	GetWorld()->GetTimerManager().SetTimer(InterpolationTimer, [this, Weapon, Socket]
	{
		ElapsedTime += 0.01f;
		float Alpha = FMath::Clamp(ElapsedTime / TotalDuration, 0.f, 1.f);
		
		FTransform End = OwnerRef->GetCharacterMesh()->GetSocketTransform(Socket, RTS_World);
		FVector Loc = FMath::VInterpTo(Weapon->GetActorLocation(), End.GetLocation(), Alpha, TotalDuration);
		FRotator Rot = FMath::RInterpTo(Weapon->GetActorRotation(), FRotator(End.GetRotation()), Alpha, TotalDuration);
		Weapon->SetActorLocation(Loc);
		Weapon->SetActorRotation(Rot);
		
		if (Alpha >= 1.f)
		{
			Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
			GetWorld()->GetTimerManager().ClearTimer(InterpolationTimer);
		}
	}, 0.01f, true);
}
#pragma endregion
