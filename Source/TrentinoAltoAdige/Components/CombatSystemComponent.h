// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatSystemComponent.generated.h"


class ICombatInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRENTINOALTOADIGE_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatSystemComponent();

	bool IsAttacking() const {return bIsAttacking;}
	void SaveCombo();
	void ResetCombo();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void Attack();
private:
	ICombatInterface* OwnerRef;
	int32 AttackIndex = 0;
	bool bIsAttacking = false;
	bool bSaveCombo = false;
};
