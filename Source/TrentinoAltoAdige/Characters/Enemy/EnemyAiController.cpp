// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAiController.h"

#include "EnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h"

// Definizione chiavi Blackboard
const FName AEnemyAiController::BBKey_TargetActor     = TEXT("TargetActor");
const FName AEnemyAiController::BBKey_PatrolLocation  = TEXT("PatrolLocation");
const FName AEnemyAiController::BBKey_EnemyState      = TEXT("EnemyState");
const FName AEnemyAiController::BBKey_LastKnownLocation = TEXT("LastKnownLocation");
const FName AEnemyAiController::BBKey_CanSeeTarget    = TEXT("CanSeeTarget");
const FName AEnemyAiController::BBKey_IsInAttackRange = TEXT("IsInAttackRange");
const FName AEnemyAiController::BBKey_IsAlerted       = TEXT("IsAlerted");

// Sets default values
AEnemyAiController::AEnemyAiController(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	SetPerceptionComponent(*PerceptionComp);
}

void AEnemyAiController::SetupPerceptionSystem()
{
	SightConfig = NewObject<UAISenseConfig_Sight>(this);
	SightConfig->SightRadius = 1200.f;
	SightConfig->LoseSightRadius = 1400.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies   = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals  = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	PerceptionComp->ConfigureSense(*SightConfig);
	
	HearingConfig = NewObject<UAISenseConfig_Hearing>(this);
	HearingConfig->HearingRange = 700.0f;
	HearingConfig->SetMaxAge(3.0f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	PerceptionComp->ConfigureSense(*HearingConfig);
	
	DamageConfig = NewObject<UAISenseConfig_Damage>(this);
	DamageConfig->SetMaxAge(5.0f);
	PerceptionComp->ConfigureSense(*DamageConfig);
	
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AEnemyAiController::OnPerceptionUpdated);
}

void AEnemyAiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn))
	{
		if (Enemy->BehaviorTree)
		{
			UBlackboardComponent* BB = nullptr;
			UseBlackboard(Enemy->BehaviorTree->BlackboardAsset, BB);
			RunBehaviorTree(Enemy->BehaviorTree);
		}
	}
}
 
// Called when the game starts or when spawned
void AEnemyAiController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyAiController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AEnemyAiController::SetBlackboardTarget(AActor* Target)
{
	if (Blackboard)
	{
		Blackboard->SetValueAsObject(BBKey_TargetActor, Target);
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
			Enemy->GetCombatSystemComponent()->CurrentTargetActor = Target;
	}
}

void AEnemyAiController::ClearBlackboardTarget()
{
	if (Blackboard)
	{
		Blackboard->ClearValue(BBKey_TargetActor);
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
			Enemy->GetCombatSystemComponent()->CurrentTargetActor = nullptr;
	}
}

void AEnemyAiController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo PerceptionInfo;
		PerceptionComp->GetActorsPerception(Actor, PerceptionInfo);
		
		for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
		{
			if (Stimulus.Type == UAISense::GetSenseID<UAISenseConfig_Sight>())
				HandleSightStimulus(Actor, Stimulus);
			else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
				HandleHearingStimulus(Actor, Stimulus);
			else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
				HandleDamageStimulus(Actor, Stimulus);
		}
	}
}

void AEnemyAiController::HandleSightStimulus(AActor* SensedActor, const FAIStimulus& Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		SetBlackboardTarget(SensedActor);
		Blackboard->SetValueAsBool(BBKey_CanSeeTarget, true);
		Blackboard->SetValueAsVector(BBKey_LastKnownLocation, SensedActor->GetActorLocation());
		Blackboard->SetValueAsBool(BBKey_IsAlerted, true);
	}
	else
	{
		Blackboard->SetValueAsBool(BBKey_CanSeeTarget, false);
	}
}

void AEnemyAiController::HandleHearingStimulus(AActor* SensedActor, const FAIStimulus& Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsVector(BBKey_LastKnownLocation, SensedActor->GetActorLocation());
		Blackboard->SetValueAsBool(BBKey_IsAlerted, true);
	}
}

void AEnemyAiController::HandleDamageStimulus(AActor* SensedActor, const FAIStimulus& Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		SetBlackboardTarget(SensedActor);
		Blackboard->SetValueAsBool(BBKey_IsAlerted, true);
	}
}