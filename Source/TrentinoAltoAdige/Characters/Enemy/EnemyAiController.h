// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "EnemyAiController.generated.h"

struct FAIStimulus;

UCLASS()
class TRENTINOALTOADIGE_API AEnemyAiController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyAiController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Blackboard Keys
	static const FName BBKey_TargetActor;
	static const FName BBKey_PatrolLocation;
	static const FName BBKey_EnemyState;
	static const FName BBKey_LastKnownLocation;
	static const FName BBKey_CanSeeTarget;
	static const FName BBKey_IsInAttackRange;
	static const FName BBKey_IsAlerted;
	
	// Metodi helper
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetBlackboardTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ClearBlackboardTarget();

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetBlackboardTarget() const {return Cast<AActor>(Blackboard->GetValueAsObject(BBKey_TargetActor));}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	class UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	class UAISenseConfig_Damage* DamageConfig;

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private:
	void SetupPerceptionSystem();
	void HandleSightStimulus(AActor* SensedActor, const FAIStimulus& Stimulus);
	void HandleHearingStimulus(AActor* SensedActor, const FAIStimulus& Stimulus);
	void HandleDamageStimulus(AActor* SensedActor, const FAIStimulus& Stimulus);
};
