// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnSlotClicked);
	}
}

void UInventorySlotWidget::SetItem(const FInventoryItem& InItem, int32 InSlotIndex)
{
	CurrentItem = InItem;
	SlotIndex = InSlotIndex;
	
	if (QuantityText)
	{
		if (InItem.ItemDescription.IsStackable())
		{
			QuantityText->SetText(
				FText::FromString(FString::Printf(TEXT("x%d"), InItem.Quantity)));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	if (ItemIcon)
	{
		if (UTexture2D* Icon = Cast<UTexture2D>(InItem.ItemDescription.IconPath.TryLoad()))
		{
			ItemIcon->SetBrushFromTexture(Icon);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	OnItemSet(InItem);
}

void UInventorySlotWidget::OnSlotClicked()
{
	if (!IsEmpty())
	{
		OnSlotClickedDelegate.Broadcast(SlotIndex, CurrentItem);
	}
}

void UInventorySlotWidget::ClearSlot()
{
	// Resetta i dati
	CurrentItem = FInventoryItem{};
	SlotIndex   = -1;
 
	// Nasconde icona e quantità
	if (ItemIcon)    ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	if (QuantityText) QuantityText->SetVisibility(ESlateVisibility::Hidden);
 
	// Notifica il Blueprint (es. per rimettere uno sfondo grigio)
	OnSlotCleared();
}
