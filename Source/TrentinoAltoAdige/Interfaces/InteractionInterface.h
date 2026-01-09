// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UENUM()
enum EInteractionType
{
	None,
	Instant,
	Duration
};

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRENTINOALTOADIGE_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Interact(AActor* Interactor) = 0;
	UFUNCTION(BlueprintCallable)
	virtual void StopInteract() = 0;
	virtual void ShowInteractionWidget() = 0;
	virtual void HideInteractionWidget() = 0;
	virtual EInteractionType GetInteractionType() = 0;
	virtual class UInputMappingContext* GetInteractionMappingContext() = 0;
};
