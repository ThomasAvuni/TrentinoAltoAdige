// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GetComponentInterface.generated.h"

class UCombatSystemComponent;
// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UGetComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRENTINOALTOADIGE_API IGetComponentInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual UCombatSystemComponent* GetCombatSystemComponent() const = 0; 
	virtual void SnapToTarget() = 0;
};
