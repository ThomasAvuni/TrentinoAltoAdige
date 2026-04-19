// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Potion.generated.h"

UCLASS()
class TRENTINOALTOADIGE_API APotion : public AItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APotion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void UseItem(AActor* User) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float HealthGiven = 30.f;
};
