// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UsableItem.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UUsableItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRENTINOALTOADIGE_API IUsableItem
{
	GENERATED_BODY()

public:
	virtual void UseItem(AActor* User) = 0;
};
