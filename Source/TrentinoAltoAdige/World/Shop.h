// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionObjectBase.h"
#include "Shop.generated.h"

class UCameraComponent;

UCLASS()
class TRENTINOALTOADIGE_API AShop : public AInteractionObjectBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	virtual void Interact(AActor* Interactor) override;
	virtual void StopInteract() override;
	virtual EInteractionType GetInteractionType() override {return Duration;}
};
