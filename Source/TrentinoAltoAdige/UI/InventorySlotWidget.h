// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TrentinoAltoAdige/Components/InventoryComponent.h"
#include "InventorySlotWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
struct FInventoryItem;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UInventorySlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetItem(const FInventoryItem& InItem, int32 InSlotIndex);
	
	UFUNCTION(BlueprintCallable)
	void ClearSlot();
	
	UFUNCTION(BlueprintPure, Category = "Slot")
	bool IsEmpty() const { return !CurrentItem.IsValid(); }
	
	UPROPERTY(BlueprintReadWrite)
	FInventoryItem CurrentItem;
	
	UPROPERTY(BlueprintReadWrite)
	int32 SlotIndex = -1;
	
protected:
	// Il nome della variabile DEVE corrispondere esattamente
	// al nome del widget nel Designer di UMG
 
	// Icona dell'item
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
 
	// Testo che mostra la quantità (es. "x3")
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QuantityText;
 
	// Pulsante cliccabile che copre l'intero slot
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;
 
	// Chiamata da UE quando il widget viene costruito
	virtual void NativeConstruct() override;
 
	// Callback del click sul pulsante
	UFUNCTION()
	void OnSlotClicked();
 
	// Chiamata dal Blueprint per aggiornare icona e testo
	// dopo che SetItem() ha impostato i dati
	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnItemSet(const FInventoryItem& Item);
 
	// Chiamata dal Blueprint per resettare l'aspetto visivo
	UFUNCTION(BlueprintImplementableEvent, Category = "Slot")
	void OnSlotCleared();
 
public:
	// Delegate che il widget padre (WBP_Inventory) ascolta
	// per sapere quale slot è stato cliccato
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
		FOnSlotClicked, int32, ClickedSlotIndex, FInventoryItem, ClickedItem);
 
	UPROPERTY(BlueprintAssignable, Category = "Slot")
	FOnSlotClicked OnSlotClickedDelegate;
};
