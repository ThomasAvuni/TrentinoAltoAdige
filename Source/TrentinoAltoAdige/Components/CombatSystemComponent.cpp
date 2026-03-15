// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatSystemComponent.h"

#include "DamageComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	OnWeaponUpgraded.Broadcast(1);
}

#pragma region Attack
void UCombatSystemComponent::Attack()
{
	// Controllo rapido: se non è equipaggiata un'arma non procedere.
	if (!OwnerRef->IsWeaponEquipped()) return;

	// Se sto già attaccando e non è stato permesso il salvataggio combo, esco.
	if (bIsAttacking && !bSaveCombo) return;

	if (bIsParrying && !bIsPerfectParrying) return;
	
	EnemiesHitThisAttack.Empty();

	// Controllo che OwnerRef sia valido (null-check) e recupero l'arma.
	if (OwnerRef)
	{
		if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
		{
			// Ottengo l'AnimInstance della mesh del personaggio (per riprodurre montage).
			if (AnimInstance)
			{
				// Assicuro che l'arma sia attaccata alla socket della mano per l'attacco.
				Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetHandSocket());
				
				if (bIsPerfectParrying)
				{
					AnimInstance->StopAllMontages(0.15f);
					AnimInstance->Montage_Play(Weapon->GetPerfectParryCounterAttack());
					bIsPerfectParrying = false;
					return;
				}
				
				// Ottengo gli attacchi a combo definiti dall'arma.
				const TArray<FAttack>& ComboAttacks = Weapon->GetWeaponComboAttacks();
				int32 CurrentAttackIndex = AttackIndex % ComboAttacks.Num();

				// Se il montage dell'attacco esiste, lo eseguo.
				if ((CurrentAttackMontage = ComboAttacks[CurrentAttackIndex].AttackMontage))
				{
					// Esegui suono d'attacco se presente.
					if (AttackSound)
						UGameplayStatics::PlaySound2D(GetWorld(), AttackSound, 0.65f);

					EnemiesHitThisAttack.Empty();
					
					// Stato: sto attaccando, reset del flag save combo.
					bIsAttacking = true;
					bSaveCombo = false;

					// Se è un player, applica effetto camera lerp durante l'attacco.
					if (PlayerOwnerRef)
						PlayerOwnerRef->LerpCamToAttackPosition();
					
					// Avvio il montage di attacco.
					AnimInstance->Montage_Play(CurrentAttackMontage);
				
					// Incremento l'indice degli attacchi per il prossimo colpo di combo.
					AttackIndex++;
				}
			}
		}
	}
}

void UCombatSystemComponent::PerformTrace()
{
	// Recupero arma e definisco start/end per lo sweep basato sui socket della mesh dell'arma.
	if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
	{
		FVector TraceStart = Weapon->GetMesh()->GetSocketLocation(FName("SwordBase"));
		FVector TraceEnd = Weapon->GetMesh()->GetSocketLocation(FName("SwordTip"));
		float fRadius = 30.f; // raggio della sfera usata per lo sweep
		FHitResult HitResult;
#if  0
		bool bDebugPersistent = false;
		float fDebugLifeTime = 2.0f;

		// 1. Calcoliamo il vettore direzione e la distanza totale
		FVector TraceVector = TraceEnd - TraceStart;
		float TraceDistance = TraceVector.Size();

		// 2. Calcoliamo il punto centrale (Midpoint)
		FVector CenterPoint = TraceStart + (TraceVector * 0.5f);

		// 3. Calcoliamo la rotazione
		// La capsula di debug è orientata verticalmente (asse Z), quindi creiamo
		// una rotazione che allinei l'asse Z alla direzione del nostro TraceVector.
		FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();

		// 4. Calcoliamo la HalfHeight
		// In Unreal, la HalfHeight include il raggio dell'emisfero.
		// Quindi è: (Metà della distanza lineare) + (Raggio)
		float HalfHeight = (TraceDistance * 0.5f) + fRadius;

		// Disegna la capsula
		DrawDebugCapsule(
			GetWorld(), 
			CenterPoint, 
			HalfHeight, 
			fRadius, 
			CapsuleRotation, 
			FColor::Blue, 
			bDebugPersistent, 
			fDebugLifeTime
		);
#endif
		//Solo oggetti di tipo Pawn (es. nemici)
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
					// if (Enemy->GetTeam() == OwnerRef->GetTeam()) return;
					
					// Registro l'attore colpito per evitare clash multipli
					EnemiesHitThisAttack.Add(HitActor);
					// Se è il player, applico camera shake
					if (PlayerOwnerRef)
						PlayerOwnerRef->CameraShake();
					// Spawn VFX se presente
					if (UNiagaraSystem* HitVFX = Enemy->IsParrying() ? HitBlockVFX : HitBloodVFX)
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFX, HitResult.Location);
					// HitSound se presente
					if (USoundBase* HitSound = Enemy->IsParrying() ? Weapon->GetWeaponBlockSound() : Weapon->GetWeaponHitSound())
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
					float Damage;
					if (Enemy->IsParrying())
					{
						float Time = GetWorld()->GetTimeSeconds();
						float DeltaTime = Time - Enemy->GetCombatSystemComponent()->GetParryStartTime();
						if (DeltaTime <= PerfectParryWindow)
						{
							// if (bCanPerfectParry)
							{
								Enemy->GetCombatSystemComponent()->bIsPerfectParrying = true;
								Enemy->HandlePerfectParry();
							}
							// else
							// {
							// 	Enemy->HandleParry();
							// }
						}
						else
						{
							Enemy->HandleParry();
						}
						Damage = CalculateDamage(Weapon->GetWeaponBaseDamage()) * 0.5f;
					}
					else
					{
						Damage = CalculateDamage(Weapon->GetWeaponBaseDamage());
					}
					
					// Calcolo direzione del colpo rispetto alla forward vector del bersaglio
					FVector HitActorForwardVector = HitActor->GetActorForwardVector();
					FVector Direction = HitActor->GetActorLocation() - GetOwner()->GetActorLocation();
					Direction.Normalize();
					float DotProduct = FVector::DotProduct(HitActorForwardVector, Direction);
					EHitDirection HitDirection = HitNone;
					if (DotProduct > 0.6f)
					{
						HitDirection = Back;
					}
					else if (DotProduct < -0.6f)
					{
						HitDirection = Front;
					}

					// Calcola rotazione dal nemico al player (owner)
					FVector EnemyLocation = HitActor->GetActorLocation();
					FVector PlayerLocation = GetOwner()->GetActorLocation();
					FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(EnemyLocation, PlayerLocation);
					// Mantieni solo la rotazione Yaw per evitare tilt strani
					FRotator CurrentRotation = HitActor->GetActorRotation();
					FRotator NewRotation(0.f, LookAtRotation.Yaw, 0.f);
					HitActor->SetActorRotation(NewRotation);
					
					FDamage sDamage;
					sDamage.DamageAmount = Damage;
					sDamage.ShouldDoDamage = !Enemy->IsParrying();
					sDamage.HitDirection = HitDirection;
					Enemy->GetDamageComponent()->TakeDamage(sDamage);
					
					// Memorizzo l'attore corrente colpito
					CurrentHitActor = bIsTargeting && CurrentTargetActor.IsValid() ? CurrentTargetActor.Get() : HitActor;
				}
			}
		}
	}
}

void UCombatSystemComponent::MoveToTarget() const
{
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (CurrentHitActor.IsValid())
		{
			float Dist = FVector::Dist(CurrentHitActor->GetActorLocation(), Char->GetActorLocation());
			if (Dist > MinDist)
			{
				FVector ForwardVector = Char->GetActorForwardVector();

				float NormalizedDist = FMath::Clamp((Dist - MinDist) / (MaxDist - MinDist), 0.f, 1.f);

				float DashStrength = FMath::Lerp(MinDashStrength, MaxDashStrength, NormalizedDist);
				FVector LaunchVelocity = (ForwardVector * DashStrength) + FVector(0.f, 0.f, 80.f);
				Char->LaunchCharacter(LaunchVelocity, true, true);
			}
		}
		else
		{
			FVector ForwardVector = Char->GetActorForwardVector();
			FVector LaunchVelocity = (ForwardVector * MaxDashStrength) + FVector(0.f, 0.f, 80.f);
			Char->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

void UCombatSystemComponent::ResetEnemiesHitThisAttack()
{
	EnemiesHitThisAttack.Empty();
}

void UCombatSystemComponent::SaveCombo()
{
	// Se attualmente in attacco, permetto di salvare la combo per concatenare l'attacco successivo.
	if (bIsAttacking)
		bSaveCombo = true;
	else
		AnimInstance->Montage_Stop(0.f, CurrentAttackMontage);
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
	ResetEnemiesHitThisAttack();
	AttackIndex = 0;

	AnimInstance->Montage_Stop(0.f, CurrentAttackMontage);
	
	// Riattacco l'arma alla socket di idle (posizione a riposo).
	if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
	{
		Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetIdleSocket());
	}
}
float UCombatSystemComponent::CalculateDamage(float WeaponBaseDamage) const
{
	float Multiplier = 1.f + (DamageMultiplierPerLevel * (CurrentWeaponLevel - 1));
	return WeaponBaseDamage * Multiplier;
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

// Gestisce l'attivazione e disattivazione del sistema di puntamento (Lock-on)
void UCombatSystemComponent::Target()
{
    // Se non stiamo mirando, inizia a cercare un bersaglio
    if (!bIsTargeting)
    {
       StartTarget();
       return;
    }
    
    // Se stavamo già mirando, interrompi il puntamento
    StopTarget();
}

void UCombatSystemComponent::StartTarget()
{
    bIsTargeting = true;
    
    // Calcola il punto di inizio per il rilevamento (leggermente spostato rispetto all'attore)
    FVector Start = GetOwner()->GetActorLocation() + FVector(-100.f, 0, 20.f);
    
    // Calcola il punto finale del raggio (End) basandosi sulla telecamera (se controllato dal player) o sulla direzione dell'attore
    FVector End = PlayerOwnerRef ? Start + (PlayerOwnerRef->GetCamera()->GetForwardVector() * 1200.f)
                          : Start + (GetOwner()->GetActorForwardVector() * 1200.f);  
    
    float CapsuleRadius = 650.f; // Raggio della sfera/capsula di ricerca
    FVector Dir = End - Start;
    float Distance = Dir.Size();
    
    TArray<FHitResult> HitResults;
    
    // Configura la query per cercare solo Pawn (personaggi/nemici)
    FCollisionObjectQueryParams QueryParams;
    QueryParams.AddObjectTypesToQuery(ECC_Pawn);
    
    // Ignora se stesso durante la ricerca
    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(GetOwner());

    // Esegue uno "Sweep" (lancio di una forma geometrica) per trovare nemici nell'area
    if (GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, QueryParams,
       FCollisionShape::MakeCapsule(CapsuleRadius, FMath::Max(10.f, Distance * 0.5f)), CollisionQueryParams))
    {
       // Variabili per tracciare il miglior bersaglio
       float ClosestDist = BIG_NUMBER;
       AActor* ClosestActor = nullptr;      // Migliore per distanza assoluta
       float ClosestInCone = BIG_NUMBER;
       AActor* BestInCone = nullptr;        // Migliore che si trova "davanti" al giocatore (nel cono visivo)

       for (FHitResult& HitResult : HitResults)
       {
          AActor* Actor = HitResult.GetActor();
          // Evita di aggiungere duplicati
          if (!TargetActors.Contains(Actor))
          {
          	if (ICombatInterface* Enemy = Cast<ICombatInterface>(Actor))
          	{
          		if (!Enemy->CanBeTargeted() || Enemy->GetDamageComponent()->GetIsDead())
          			return;
          	}
             TargetActors.AddUnique(Actor);
          
             // Logica Matematica per capire se il nemico è davanti a noi
             FVector Direction = Actor->GetActorLocation() - GetOwner()->GetActorLocation();
             Direction.Normalize();
             FVector FacingVector = PlayerOwnerRef ? PlayerOwnerRef->GetCamera()->GetForwardVector() : GetOwner()->GetActorForwardVector();
             
             // DotProduct: 1.0 = esattamente davanti, 0.0 = di lato, -1.0 = dietro
             float DotProduct = FVector::DotProduct(Direction, FacingVector);
             
             // Soglia dell'angolo (circa 35 gradi per lato)
             float Angle = FMath::Cos(FMath::DegreesToRadians(35.f));
             float Dist = FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
             
             // Se il nemico è FUORI dal cono visivo (o dietro), consideralo solo per la distanza
             if (DotProduct < Angle)
             {
                if (Dist < ClosestDist)
                {
                   ClosestDist = Dist;
                   ClosestActor = Actor;
                }
                continue;
             }

             // Se il nemico è DENTRO il cono visivo, ha priorità
             if (Dist < ClosestInCone)
             {
                ClosestInCone = Dist;
                BestInCone = Actor;
             }
          }
       }
       // Preferisce chi è nel cono visivo; se nessuno è davanti, prende il più vicino in assoluto
       CurrentTargetActor = BestInCone ? BestInCone : ClosestActor;
    }

    // Se abbiamo trovato un bersaglio valido
    if (CurrentTargetActor.IsValid())
    {
	    ACharacter* Char = GetOwner<ACharacter>();
       
    	// Controllo distanza massima di sicurezza (15 metri)
    	float Dist = FVector::Dist(Char->GetActorLocation(), CurrentTargetActor->GetActorLocation());
    	if (Dist > 1500.f)
    	{
    		CurrentTargetActor = nullptr;
    		return;
    	}
    	
       
    	// Mostra l'icona di lock-on sul nemico (se implementa l'interfaccia)
    	if (ICombatInterface* Enemy = Cast<ICombatInterface>(CurrentTargetActor))
    	{
    		if (Enemy->GetDamageComponent()->GetIsDead() || !Enemy->CanBeTargeted())
    		{
    			StopTarget();
    			return;
    		}
    		
    		Enemy->ShowTargetWidget();
          
    		// Disabilita l'input del mouse per la telecamera (Hard Lock)
    		Char->GetController()->SetIgnoreLookInput(true);
       
    		// Avvia un Timer Loop (ogni 0.01s) per ruotare la telecamera/personaggio verso il nemico
    		GetWorld()->GetTimerManager().SetTimer(LerpToTargetActorTimer, [this, Char, Enemy]
			{
    			if (Enemy)
    			{
    				if (Enemy->GetDamageComponent()->GetIsDead() || !Enemy->CanBeTargeted())
    				{
    					GetWorld()->GetTimerManager().ClearTimer(LerpToTargetActorTimer);
    					StopTarget();
					}
    			}
			   if (Char && CurrentTargetActor.IsValid()) // Aggiunto check validità CurrentTargetActor
			   {
				  float Dist = FVector::Dist(Char->GetActorLocation(), CurrentTargetActor->GetActorLocation());
				  // Se il nemico si allontana troppo, sgancia il target
				  if (Dist > 1500.f)
				  {
					 StopTarget();
					 return;
				  }
             
				  FVector TargetLocation = CurrentTargetActor->GetActorLocation();
				  // Aggiusta l'altezza del target point (mira al torso/piedi)
				  if (!bIsAttacking)
				  {
					 TargetLocation.Z -= 20.f;
				  }
             
				  // Calcola la rotazione necessaria per guardare il nemico
				  FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);
             
				  // Interpola fluidamente la rotazione corrente verso quella desiderata
				  FRotator InterpRot = FMath::RInterpTo(Char->GetControlRotation(), LookAt, GetWorld()->GetDeltaSeconds(), 10.f);
				  Char->GetController()->SetControlRotation(InterpRot);
			   }
			}, 0.01f, true);
    	}
    }
}

// Passa al prossimo bersaglio disponibile nella lista trovata
void UCombatSystemComponent::NextTarget()
{
    if (TargetActors.IsEmpty())
    {
       bIsTargeting = false;
       CurrentTargetActor = nullptr;
       return;
    }
    
    // Nasconde il widget sul vecchio bersaglio
    if (ICombatInterface* OldTargetActor = Cast<ICombatInterface>(CurrentTargetActor))
    {
       OldTargetActor->HideTargetWidget();
    }
    
    // Incrementa l'indice ciclicamente (modulo %)
    int32 NewIndex = ++TargetIndex % TargetActors.Num();
    if (AActor* CandidateActor = TargetActors[NewIndex])
    {
       CurrentTargetActor = CandidateActor;
       // Mostra il widget sul nuovo bersaglio
       if (ICombatInterface* NewTargetActor = Cast<ICombatInterface>(CurrentTargetActor))
       {
          NewTargetActor->ShowTargetWidget();
       }
    }
    else
       CurrentTargetActor = nullptr;
}

// Resetta tutto il sistema di targeting
void UCombatSystemComponent::StopTarget()
{
    if (ICombatInterface* Enemy = Cast<ICombatInterface>(CurrentTargetActor))
       Enemy->HideTargetWidget();
       
    bIsTargeting = false;
    TargetActors.Empty();
    CurrentTargetActor = nullptr;
    
    // IMPORTANTE: Ferma il timer di rotazione
    GetWorld()->GetTimerManager().ClearTimer(LerpToTargetActorTimer);
    
    // Restituisce il controllo della telecamera al giocatore
    ACharacter* Char = GetOwner<ACharacter>();
    if(Char && Char->GetController()) // Check di sicurezza
        Char->GetController()->SetIgnoreLookInput(false);
}
#pragma endregion

#pragma region Equipping
// Gestisce l'animazione e la logica per sfoderare l'arma
void UCombatSystemComponent::EquipWeapon(FName InSocket, UAnimMontage* EquipMontage)
{
    if (AnimInstance && EquipMontage)
    {
       bIsEquippingWeapon = true;
       AnimInstance->Montage_Play(EquipMontage);
       
       FOnMontageEnded OnMontageEnded;
       // Lambda eseguita quando l'animazione finisce
       OnMontageEnded.BindLambda([this, InSocket](UAnimMontage* Montage, bool bInterrupted)
       {
          // Solo alla FINE dell'animazione (presumibilmente quando la mano ha afferrato l'arma)
          // spostiamo l'arma nello slot della mano (InSocket)
          if (OwnerRef && OwnerRef->GetWeapon()) // Check sicurezza OwnerRef
          {
             AWeaponBase* Weapon = OwnerRef->GetWeapon();
             Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocket);
             bIsEquippingWeapon = false;
             bIsWeaponEquipped = true;
          }
       });
       AnimInstance->Montage_SetEndDelegate(OnMontageEnded, EquipMontage);
    }
}

// Gestisce l'animazione e la logica per rinfoderare l'arma
void UCombatSystemComponent::UnEquipWeapon(FName InSocket, UAnimMontage* UnEquipMontage)
{
    if (bIsAttacking) return; // Non rinfoderare se stai attaccando
    
    if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
    {
       if (AnimInstance && UnEquipMontage)
       {
          // NOTA: Qui l'arma viene attaccata al fodero (InSocket) IMMEDIATAMENTE,
          // prima ancora che l'animazione inizi. Visivamente potrebbe sembrare un "teletrasporto"
          // se l'animazione non prevede che la mano sia già sul fodero.
          Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocket);
          
          AnimInstance->Montage_Play(UnEquipMontage);
          bIsEquippingWeapon = true;
          bIsWeaponEquipped = false;
          
          FOnMontageEnded OnMontageEnded;
          OnMontageEnded.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
          {
             bIsEquippingWeapon = false;
             OnWeaponUnEquipped.Broadcast(); // Notifica UI/Inventario
          });
          AnimInstance->Montage_SetEndDelegate(OnMontageEnded, UnEquipMontage);
       }
    }
}
#pragma endregion

#pragma region Parry
void UCombatSystemComponent::StartParry()
{
    // Blocca parry multipli o se l'arma non è in mano
    if (bIsParrying || !bIsWeaponEquipped) return;

	ParryStartTime = GetWorld()->GetTimeSeconds();
	
    bIsParrying = true;
    if (OwnerRef)
    {
       // Rallenta il personaggio mentre para
       OwnerRef->SetMovementToWalk();
       // NOTA: Sembra resettare l'arma all'IdleSocket durante il parry?
       // Potrebbe essere intenzionale per specifiche animazioni.
       if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
       {
          Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetIdleSocket());
       }
    }
}

void UCombatSystemComponent::EndParry()
{
    if (!bIsParrying)
       return;
    bIsParrying = false;
    if (OwnerRef)
    {
       OwnerRef->ResetMovement(); // Ripristina velocità normale
       if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
       {
           // Anche qui riattacca all'IdleSocket. Assicurarsi che sia il socket corretto.
          Weapon->AttachToComponent(OwnerRef->GetCharacterMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetIdleSocket());
       }
    }
}
#pragma endregion

#pragma region WeaponLeveling
// Tenta di aumentare il livello dell'arma 
void UCombatSystemComponent::UpgradeWeapon(/*!SOSTITUIRE CON INVENTORYCOMPONENT*/ int32 NumberOfShards)
{
    int32 CostNeededForUpgrade = GetUpgradeCostForLevel(CurrentWeaponLevel);
    
    // Controlla se abbiamo raggiunto il level cap
    if (CurrentWeaponLevel < MaxWeaponLevel)
    {
       if (NumberOfShards >= CostNeededForUpgrade)
       {
          CurrentWeaponLevel++;
          // Qui dovresti inserire la logica per rimuovere le shard dall'inventario
          UpdateWeaponMesh(CurrentWeaponLevel);
          OnWeaponUpgraded.Broadcast(CurrentWeaponLevel);
       }
       else
       {
          // Gestione errore: risorse insufficienti
          int32 Diff = CostNeededForUpgrade - NumberOfShards;
          FString Messaggio = FString::Printf(TEXT("Risorse insufficienti: servono %d shards, ne hai %d. Mancanti: %d"), CostNeededForUpgrade, NumberOfShards, Diff);
          OnWeaponFailedUpgrade.Broadcast(Messaggio);
       }
    }
    else
    {
       // Gestione errore: livello massimo
       FString Messaggio = FString::Printf(TEXT("Arma già al livello massimo (%d)"), MaxWeaponLevel);
       OnWeaponFailedUpgrade.Broadcast(Messaggio);
    }
}

// Cambia la Mesh dell'arma in base al livello (es. Spada di ferro -> Spada d'oro)
void UCombatSystemComponent::UpdateWeaponMesh(int32 WeaponLevel)
{
    if (!WeaponMeshes.IsEmpty())
    {
       if (AWeaponBase* Weapon = OwnerRef->GetWeapon())
       {
          // L'array è base-0, i livelli partono da 1 solitamente
          int32 NewMeshIndex = WeaponLevel - 1;
          if (WeaponMeshes.IsValidIndex(NewMeshIndex))
          {
             if (USkeletalMesh* NewMesh = WeaponMeshes[NewMeshIndex])
             {
                Weapon->SetNewLevelMesh(NewMesh);
             }
          }
       }
    }
}

// Calcola il costo esponenziale per l'upgrade
// Formula: CostoBase * (Moltiplicatore ^ (Livello - 1))
int32 UCombatSystemComponent::GetUpgradeCostForLevel(int32 Level) const
{
    return FMath::RoundToInt(BaseCostForUpgrade * FMath::Pow(CostMultiplier, Level - 1));
}
#pragma endregion
