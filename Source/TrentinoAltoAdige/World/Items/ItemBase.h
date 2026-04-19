// Fill out your copyright notice in the Description page of Project Settings.

//AItemBase è l'oggetto con cui si può interagire, quello che viene droppato per terra, in generale quello visibile.
//Mentre FItemDescription è quello che conserva tutte le informazioni dell'oggetto e di quando sarà nell'inventario

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	ItemBase,
	NormalItem,
	Consumable,
	Weapon,
	Ability,
	QuestItem
};

USTRUCT(BlueprintType)
struct FItemDescription
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemID = "item:baseItem";    
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString DisplayName = "Item Base";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Description = "Item Description";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType = EItemType::ItemBase;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FSoftObjectPath IconPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize = 8;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanBeDropped = true;
	
	//Operator "==" overload
	bool operator==(const FItemDescription& Other) const
	{
		return ItemID == Other.ItemID;
	}
	
	bool IsStackable() const
	{
		return MaxStackSize > 1;
	}
	
};

UCLASS()
class TRENTINOALTOADIGE_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemBase();

	UFUNCTION(BlueprintCallable)
	const FItemDescription& GetItemDescription() const {return ItemDescription;}
	
	//Funzione base che verrà implementata per ogni sottoclasse, che viene chiamata quando si prova a usare l'oggetto
	virtual void UseItem(AActor* User);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FItemDescription ItemDescription;
};
