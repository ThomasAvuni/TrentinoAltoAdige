// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrentinoAltoAdige/World/Items/ItemBase.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FItemDescription ItemDescription;
	
	UPROPERTY(EditAnywhere)
	int32 Quantity = 1;

	bool IsValid() const
	{
		return !ItemDescription.ItemID.IsEmpty() && Quantity > 0;
	}

	bool IsStackFull() const
	{
		return Quantity >= ItemDescription.MaxStackSize;
	}
	
	bool operator==(const FInventoryItem& Other) const
	{
		return Other.ItemDescription.ItemID == ItemDescription.ItemID;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRENTINOALTOADIGE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	int32 MaxSlots = 20;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, FInventoryItem, Item, int32, Quantity);
	UPROPERTY(BlueprintAssignable)
	FOnItemAdded OnItemAdded;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, FInventoryItem, Item, int32, Quantity);
	UPROPERTY(BlueprintAssignable)
	FOnItemRemoved OnItemRemoved;
	
	// ---------------------------------------------------------------
	//  Core API
	// ---------------------------------------------------------------
 

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(const FInventoryItem& Item);
 

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItemByID(FString ItemID, int32 Amount = 1);
 
	/**
	 * Remove an item at a specific slot index.
	 * Returns the number of units actually removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItemAtIndex(int32 SlotIndex, int32 Amount = 1);
	
	/**
	  * Usa un consumabile allo slot SlotIndex.
	  * spawna un oggetto temporaneo per eseguire UseItem().
	  * Se l'uso va a buon fine, decrementa la quantità di 1.
	  * Ritorna true se l'item è stato usato correttamente.
	  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItemAtIndex(int32 SlotIndex);
 
	/** Versione per ID — usa la prima occorrenza trovata. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItemByID(FString ItemID);

	/**
	* Registra la classe Actor da spawnare quando si usa un item con quel ID.
	* Chiamala nel BeginPlay del tuo Character per ogni consumabile.
	* Es: InventoryComponent->RegisterItemClass("potion_health", AHealthPotion::StaticClass());
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RegisterItemClass(FString ItemID, TSubclassOf<AItemBase> ItemClass);
 
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FString ItemID, int32 Amount = 1) const;
 
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(FString ItemID) const;
 
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 FindItemIndexByID(FString ItemID) const;
 
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventoryItem GetItemAtIndex(int32 SlotIndex) const;
 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();
	
	FInventoryItem GetItemFromID(FString ItemID);
	
	int32 GetSlotCount() const {return InventorySlots.Num();}
	bool IsFull() const {return InventorySlots.Num() + 1 >= MaxSlots;}
	
	const TArray<FInventoryItem>& GetItems() const {return InventorySlots;}
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Debug")
	void DebugPrintInventory() const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	TArray<FInventoryItem> InventorySlots;
	
	// Mappa ItemID -> classe Actor da spawnare per UseItem
	TMap<FString, TSubclassOf<class AItemBase>> ItemClassMap;
 
	bool CanAddSlot() const;
};
