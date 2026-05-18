// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "TrentinoAltoAdige/Interfaces/UsableItem.h"
#include "TrentinoAltoAdige/World/Items/Potion.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	RegisterItemClass("potion", APotion::StaticClass());
}

void UInventoryComponent::DebugPrintInventory() const
{
	UE_LOG(LogTemp, Log, TEXT("=== Inventory (%d/%d slots) ==="),
	InventorySlots.Num(), MaxSlots);
 
	for (int32 i = 0; i < InventorySlots.Num(); ++i)
	{
		const FInventoryItem& Item = InventorySlots[i];
		UE_LOG(LogTemp, Log, TEXT("  [%d] %s  x%d "),
			i, *Item.ItemDescription.ItemID, Item.Quantity);
	}
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UInventoryComponent::CanAddSlot() const
{
	return GetSlotCount() < MaxSlots;
}

int32 UInventoryComponent::AddItem(const FInventoryItem& Item)
{
	if (Item.IsValid())
	{
		int32 RemainingToAdd = Item.Quantity;

		if (Item.ItemDescription.IsStackable())
		{
			for (FInventoryItem& Slot : InventorySlots)
			{
				if (Slot == Item)
				{
					if (Slot.Quantity <= Slot.ItemDescription.MaxStackSize)
					{
						const int32 Space = Slot.ItemDescription.MaxStackSize - Slot.Quantity;
						const int32 AddNow = FMath::Min(Space, RemainingToAdd);
						
						Slot.Quantity += AddNow;
						RemainingToAdd -= AddNow;
						
						if (RemainingToAdd == 0) break;
					}
				}
			}
		}
		
		while(RemainingToAdd > 0)
		{
			const int32 AddNow = FMath::Min(Item.ItemDescription.MaxStackSize, RemainingToAdd);
			FInventoryItem NewSlot = Item;
			NewSlot.Quantity = AddNow;
			InventorySlots.Add(NewSlot);
			RemainingToAdd -= AddNow;
		}

		const int32 ActuallyAdded = Item.Quantity - RemainingToAdd;
		if (ActuallyAdded > 0)
		{
			OnItemAdded.Broadcast(Item, ActuallyAdded);
			OnInventoryChanged.Broadcast();
		}
		
		return ActuallyAdded;
	}
	
	return 0;
}

int32 UInventoryComponent::RemoveItemByID(FString ItemID, int32 Quantity)
{
	if (!ItemID.IsEmpty() && Quantity >= 1)
	{
		int32 RemainingToRemove = Quantity;

		for (int32 i = GetSlotCount() - 1; i >= 0; --i)
		{
			if (InventorySlots[i].ItemDescription.ItemID == ItemID)
			{
				const int32 RemovedNow = FMath::Min(RemainingToRemove, Quantity);
				InventorySlots[i].Quantity -= RemovedNow;
				RemainingToRemove		   -= RemovedNow;
				
				if (InventorySlots[i].Quantity <= 0)
				{
					InventorySlots.RemoveAt(i);
				}
			}
		}

		const int32 ActuallyRemoved = Quantity - RemainingToRemove;
		
		if (ActuallyRemoved > 0)
		{
			FInventoryItem Dummy;
			Dummy.ItemDescription.ItemID = ItemID;
			OnItemRemoved.Broadcast(Dummy, ActuallyRemoved);
			OnInventoryChanged.Broadcast();
		}
		
		return ActuallyRemoved;
	}
	
	return 0;
}

int32 UInventoryComponent::RemoveItemAtIndex(int32 Index, int32 Amount)
{
	if (InventorySlots.IsValidIndex(Index) && Amount > 0)
	{
		FInventoryItem& Slot = InventorySlots[Index];
		const int32 RemoveNow = FMath::Min(Slot.Quantity, Amount);
		Slot.Quantity -= RemoveNow;
		if (Slot.Quantity <= 0)
		{
			InventorySlots.RemoveAt(Index);
		}
		OnItemRemoved.Broadcast(Slot, RemoveNow);
		OnInventoryChanged.Broadcast();
		return RemoveNow;
	}
	
	return 0;
}

FInventoryItem UInventoryComponent::GetItemFromID(FString ItemID)
{
	for (const FInventoryItem& Slot : InventorySlots)
	{
		if (Slot.ItemDescription.ItemID == ItemID)
			return Slot;
	}
	
	return FInventoryItem{};
}

bool UInventoryComponent::UseItemAtIndex(int32 Index)
{
	if (InventorySlots.IsValidIndex(Index))
	{
		FInventoryItem& Item = InventorySlots[Index];
		if (Item.ItemDescription.ItemType == EItemType::Consumable)
		{
			if (TSubclassOf<AItemBase> ItemClass = ItemClassMap[Item.ItemDescription.ItemID])
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				if (AItemBase* ItemActor = GetWorld()->SpawnActor<AItemBase>(ItemClass, FTransform::Identity, Params))
				{
					if (IUsableItem* ItemInterface = Cast<IUsableItem>(ItemActor))
					{
						ItemInterface->UseItem(GetOwner());
						RemoveItemAtIndex(Index, 1);
						ItemActor->Destroy();
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UInventoryComponent::UseItemByID(FString ItemID)
{
	const int32 Index = FindItemIndexByID(ItemID);
	return Index != INDEX_NONE ? UseItemAtIndex(Index) : false;
}

void UInventoryComponent::RegisterItemClass(FString ItemID, TSubclassOf<AItemBase> ItemClass)
{
	if (!ItemID.IsEmpty() && ItemClass)
	{
		ItemClassMap.Add(ItemID, ItemClass);
	}
}

bool UInventoryComponent::HasItem(FString ItemID, int32 Amount) const
{
	return GetItemQuantity(ItemID) >= Amount;
	
}

int32 UInventoryComponent::GetItemQuantity(FString ItemID) const
{
	int32 Total = 0;
	for (const FInventoryItem& Slot : InventorySlots)
	{
		if (Slot.ItemDescription.ItemID == ItemID)
			Total += Slot.Quantity;
	}
	
	return Total;
}

int32 UInventoryComponent::FindItemIndexByID(FString ItemID) const
{
	for (int i = 0; i < InventorySlots.Num(); ++i)
	{
		if (InventorySlots[i].ItemDescription.ItemID == ItemID)
			return i;
	}
	
	return INDEX_NONE;
}

FInventoryItem UInventoryComponent::GetItemAtIndex(int32 SlotIndex) const
{
	if (InventorySlots.IsValidIndex(SlotIndex))
	{
		return InventorySlots[SlotIndex];
	}
	
	return FInventoryItem{};
}

void UInventoryComponent::ClearInventory()
{
	InventorySlots.Empty();
}

