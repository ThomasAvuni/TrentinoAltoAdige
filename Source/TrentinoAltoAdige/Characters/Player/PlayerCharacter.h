// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;

UCLASS()
class TRENTINOALTOADIGE_API APlayerCharacter : public ACharacter
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

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* CameraBoom;

private:
	//Input actions
	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
	UInputAction* MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
	UInputAction* LookUp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
	UInputAction* LookRight;

	//Functions
	void Move(const FInputActionValue& InputValue);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
