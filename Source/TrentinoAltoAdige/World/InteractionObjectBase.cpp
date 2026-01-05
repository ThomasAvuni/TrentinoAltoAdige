// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionObjectBase.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AInteractionObjectBase::AInteractionObjectBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InteractionCollisionSphere = CreateDefaultSubobject<USphereComponent>("Interaction Sphere");	
	RootComponent = InteractionCollisionSphere;
	InteractionCollisionSphere->SetCollisionProfileName(TEXT("Custom"));
	InteractionCollisionSphere->SetGenerateOverlapEvents(true);
	InteractionCollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>("Interaction Widget");
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize({32, 32});
	InteractionWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AInteractionObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractionObjectBase::Interact(AActor* Interactor)
{
}

void AInteractionObjectBase::StopInteract()
{
}

void AInteractionObjectBase::ShowInteractionWidget()
{
	if (InteractionWidget)
		InteractionWidget->SetVisibility(true);
}

void AInteractionObjectBase::HideInteractionWidget()
{
	if (InteractionWidget)
		InteractionWidget->SetVisibility(false);
}

EInteractionType AInteractionObjectBase::GetInteractionType()
{
	return None;
}

