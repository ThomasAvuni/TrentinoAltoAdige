// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrentinoAltoAdige/Interfaces/InteractionInterface.h"
#include "InteractionObjectBase.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class TRENTINOALTOADIGE_API AInteractionObjectBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractionObjectBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* InteractionCollisionSphere;
	
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* InteractionWidget;
	
public:
	virtual void Interact(AActor* Interactor) override;
	virtual void StopInteract() override;
	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;
	virtual EInteractionType GetInteractionType() override;
	virtual UInputMappingContext* GetInteractionMappingContext() override {return nullptr;}
};
