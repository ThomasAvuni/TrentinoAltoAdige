// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

struct FInventoryItem;
class UInventorySlotWidget;
class UUniformGridPanel;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitInventory(UInventoryComponent* InInventoryComponent);
	
	UFUNCTION(BlueprintCallable)
	void RefreshInventory();	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> SlotGrid;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	int32 GridColumns = 5;
	
private:
	void BuildGrid();
	
	UFUNCTION()
	void OnSlotClicked(int32 ClickedSlotIndex, FInventoryItem ClickedItem);
};
