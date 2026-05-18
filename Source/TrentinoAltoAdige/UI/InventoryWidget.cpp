// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "TrentinoAltoAdige/Components/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::InitInventory(UInventoryComponent* InInventoryComponent)
{
	if (InInventoryComponent)
	{
		InventoryComponent = InInventoryComponent;
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::RefreshInventory);
		RefreshInventory();
	}
	
	BuildGrid();
}

void UInventoryWidget::RefreshInventory()
{
	BuildGrid();
}

void UInventoryWidget::BuildGrid()
{
	if (SlotGrid && SlotWidgetClass)
	{
		SlotGrid->ClearChildren();
		
		const TArray<FInventoryItem>& Items = InventoryComponent->GetItems();
		const int32 TotalSlots = InventoryComponent->MaxSlots > 0 ? InventoryComponent->MaxSlots : Items.Num();
		for (int32 i = 0; i < TotalSlots; i++)
		{
			UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
			
			if (SlotWidget)
			{
				if (Items.IsValidIndex(i))
				{
					SlotWidget->SetItem(Items[i], i);
				} else
				{
					SlotWidget->ClearSlot();
				}
				
				SlotWidget->OnSlotClickedDelegate.AddDynamic(this, &UInventoryWidget::OnSlotClicked);
				
				const int32 Row = i / GridColumns;
				const int32 Column = i % GridColumns;
				
				UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
			}
		}
	}
}

void UInventoryWidget::OnSlotClicked(int32 ClickedSlotIndex, FInventoryItem ClickedItem)
{
	if (IsValid(InventoryComponent))
	{
		if (ClickedItem.ItemDescription.ItemType == EItemType::Consumable)
		{
			InventoryComponent->UseItemAtIndex(ClickedSlotIndex);
		}
	}
}
