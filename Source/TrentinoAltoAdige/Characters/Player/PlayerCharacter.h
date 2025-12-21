// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TrentinoAltoAdige/Interfaces/GetComponentInterface.h"

#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;

UCLASS()
class TRENTINOALTOADIGE_API APlayerCharacter : public ACharacter, public IGetComponentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Default mapping context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Components | Combat")
	UCombatSystemComponent* CombatSystemComponent;
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Interfaces
	virtual UCombatSystemComponent* GetCombatSystemComponent() const override {return CombatSystemComponent;}
	
protected:
	UFUNCTION(BlueprintCallable)
	void BP_StartSprint();
	UFUNCTION(BlueprintCallable)
	void BP_StopSprint();
	
	//Weapon
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;
};

