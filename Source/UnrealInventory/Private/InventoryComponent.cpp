// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

#include "Item.h"

#include <GameFramework/PlayerController.h>
#include <Kismet/KismetMathLibrary.h>
#include <Net/UnrealNetwork.h>

UCInventoryComponent::UCInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UCInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Reserve some memory for the inventory ahead of time to avoid a ton of allocations later
	m_Inventory.Reserve(UnrealInventory::Items::DefaultArrayItemReserveSize);
}

void UCInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCInventoryComponent, m_EquippableInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UCInventoryComponent, m_Inventory, COND_OwnerOnly);
}

bool UCInventoryComponent::AddItem(const FCItem& Item, const ECItemSlot Slot)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't add an item to the inventory without authority"));
		return false;
	}

	if (!CanAddItemToSlot(Item.ItemDescriptor, Slot))
	{
		return false;
	}

	// Just add the item to the inventory
	if (Slot == ECItemSlot::None)
	{
		// Don't attempt to stack items that have a score
		if (Item.Score == INDEX_NONE)
		{
			// Find out where the item already exists so we can stack the item
			const TArray<UCInventoryComponent::FCItemLocation> ExistingItems = FindItemLocation(Item.ItemDescriptor);

			int32 Quantity = Item.Quantity;
			const int32 MaxQuantity = Item.ItemDescriptor->GetStackSize();

			for (const auto& ItemLocation : ExistingItems)
			{
				if (Quantity <= 0)
				{
					break;
				}

				const int32 MatchedItemQuantity = ItemLocation.Quantity;

				// Skip this iteration and try to add the remaining into another slot
				if (MatchedItemQuantity == MaxQuantity)
				{
					continue;
				}

				FCItem& ExistingItem = m_Inventory[ItemLocation.Index];

				// Don't stack items that don't match rarity or don't have a score
				if (ExistingItem.Rarity == Item.Rarity && ExistingItem.Score == INDEX_NONE)
				{
					const int32 AmountToAdd = FMath::Min(Quantity, MaxQuantity - ExistingItem.Quantity);
					const int32 NewQuantity = MatchedItemQuantity + AmountToAdd;

					ExistingItem.Quantity = NewQuantity;
					Quantity -= AmountToAdd;

					if (Quantity <= 0)
					{
						return true;
					}
				}
			}

			// Create multiple new items in the inventory
			if (Quantity > 0 && Quantity > MaxQuantity)
			{
				const int32 AmountOfItemsToCreate = UKismetMathLibrary::FCeil(Quantity / (float)MaxQuantity);

				for (int32 i = 0; i < AmountOfItemsToCreate; ++i)
				{
					if (Quantity <= 0)
					{
						break;
					}

					const int32 AmountToAdd = FMath::Min(Quantity, MaxQuantity);

					Quantity -= AmountToAdd;

					// Copy the item and modify the quantity
					FCItem CopiedItem = Item;
					CopiedItem.Quantity = AmountToAdd;

					m_Inventory.Emplace(CopiedItem);
				}

				return true;
			}
		}

		return m_Inventory.Emplace(Item) != INDEX_NONE;
	}
	else if (HasItemInEquippableSlot(Slot))
	{
		// if the item is in an equippable slot then we should replace the item in the slot
		FCItem& ItemToMove = m_EquippableInventory[static_cast<uint8>(Slot)];
		m_Inventory.Emplace(ItemToMove);
		ItemToMove = Item;

		return true;
	}

	return false;
}

bool UCInventoryComponent::RemoveItem(const FCItem& Item, const int32 Quantity, const ECItemSlot Slot)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't remove an item from the inventory without authority"));
		return false;
	}

	return false;
}

bool UCInventoryComponent::DropItem(FCItem& Item, const int32 Quantity)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't drop an item from the inventory without authority"));
		return false;
	}

	if (!Item.IsItemValid())
	{
		return false;
	}

	if (ACItemActor* Pickup = CreatePickup(Item, Quantity))
	{
		if (Quantity == INDEX_NONE || Quantity >= Item.Quantity)
		{
			const ECItemSlot Slot = GetItemSlot(Item, true);
			if (Slot == ECItemSlot::None)
			{
				// Get the index of the item
				int32 Index = INDEX_NONE;
				m_Inventory.Find(Item, Index);

				if (Index != INDEX_NONE)
				{
					m_Inventory.RemoveAt(Index);
				}
			}
			else
			{
				Item.Reset();
			}
		}
		else
		{
			const int32 ItemQuantity = Item.Quantity;
			const int32 AmountToDrop = FMath::Min(ItemQuantity, Quantity);
			const int32 NewStackSize = ItemQuantity - AmountToDrop;

			Item.Quantity = NewStackSize;
		}

		return true;
	}

	return false;
}

bool UCInventoryComponent::TradeItems(const TArray<FCItem>& ItemsToTrade, UCInventoryComponent* InventoryReceiver)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't trade items without authority"));
		return false;
	}

	if (InventoryReceiver == nullptr || InventoryReceiver == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't trade items if the inventory you're trading with is null or with yourself."));
		return false;
	}

	for (const auto& Item : ItemsToTrade)
	{
		if (MoveItemToInventory(Item, InventoryReceiver))
		{
			continue;
		}
	}

	return true;
}

bool UCInventoryComponent::HasItemInEquippableSlot(const ECItemSlot Slot) const
{
	return Slot != ECItemSlot::None && m_EquippableInventory[static_cast<uint8>(Slot)].IsItemValid();
}

bool UCInventoryComponent::CanAddItemToSlot(const UCItemDescriptorBase* Item, const ECItemSlot Slot) const
{
	if (Item != nullptr)
	{
		if (m_Inventory.Num() >= UnrealInventory::Items::MaxItems)
		{
			return false;
		}

		if (GetTotalWeight() >= UnrealInventory::Items::MaxWeight)
		{
			return false;
		}

		if (Slot == ECItemSlot::None)
		{
			return true;
		}

		const bool bHasSlot = (Item->GetItemSlot() & (1 << static_cast<int32>(Slot))) != 0;
		if (bHasSlot)
		{
			return true;
		}
	}

	return false;
}

float UCInventoryComponent::GetTotalWeight() const
{
	float Weight = 0.0f;

	for (const auto& InventoryItem : m_Inventory)
	{
		Weight += InventoryItem.GetTotalWeight();
	}

	return Weight;
}

bool UCInventoryComponent::GetItemsFromCategory(const ECItemCategory Category, TArray<FCItem>& OutItems)
{
	OutItems.Empty();
	OutItems.Reserve(UnrealInventory::TemporaryArrayReserveSize);

	for (const auto& InventoryItem : m_Inventory)
	{
		if (InventoryItem.ItemDescriptor->GetItemCategory() == Category)
		{
			OutItems.Add(InventoryItem);
		}
	}

	return OutItems.Num() > 0;
}

const FCItem& UCInventoryComponent::GetItemByIndex(const ECItemSlot Slot, const int32 Index)
{
	static FCItem DefaultItem;
	if (Slot == ECItemSlot::None)
	{
		return m_Inventory.IsValidIndex(Index) ? m_Inventory[Index] : DefaultItem;
	}

	const uint8 Idx = static_cast<uint8>(Slot);
	return m_EquippableInventory[Idx].IsItemValid() ? m_EquippableInventory[Idx] : DefaultItem;
}

int32 UCInventoryComponent::GetMatchingItemCount(const UCItemDescriptorBase* ItemToFind) const
{
	int32 Count = 0;
	for (const auto& InventoryItem : m_EquippableInventory)
	{
		if (ItemToFind == InventoryItem.ItemDescriptor)
		{
			Count += InventoryItem.Quantity;
		}
	}

	for (const auto& InventoryItem : m_Inventory)
	{
		if (ItemToFind == InventoryItem.ItemDescriptor)
		{
			Count += InventoryItem.Quantity;
		}
	}

	return Count;
}

ECItemSlot UCInventoryComponent::GetItemSlot(const FCItem& Item, const bool bSearchEquippables) const
{
	int32 Index = INDEX_NONE;
	m_Inventory.Find(Item, Index);

	if (Index != INDEX_NONE)
	{
		return ECItemSlot::None;
	}

	if (bSearchEquippables)
	{
		for (const ECItemSlot Slot : TEnumRange<ECItemSlot>())
		{
			const FCItem& InventoryItem = m_EquippableInventory[static_cast<uint8>(Slot)];
			if (Item == InventoryItem)
			{
				return Slot;
			}
		}
	}

	return ECItemSlot::Invalid;
}

int32 UCInventoryComponent::GetItemIndex(const FCItem& Item) const
{
	int32 Index = INDEX_NONE;
	m_Inventory.Find(Item, Index);
	return Index;
}

TArray<UCInventoryComponent::FCItemLocation> UCInventoryComponent::FindItemLocation(const UCItemDescriptorBase* Item, const bool bSearchEquippables) const
{
	TArray<UCInventoryComponent::FCItemLocation> TmpLocations;
	TmpLocations.Reserve(UnrealInventory::TemporaryArrayReserveSize);

	for (int32 i = 0; i < m_Inventory.Num(); ++i)
	{
		const FCItem& InventoryItem = m_Inventory[i];
		if (Item == InventoryItem.ItemDescriptor)
		{
			TmpLocations.Add({i, ECItemSlot::None, InventoryItem.Quantity});
		}
	}

	if (bSearchEquippables)
	{
		for (const ECItemSlot Slot : TEnumRange<ECItemSlot>())
		{
			const int32 Index = static_cast<uint8>(Slot);
			const FCItem& InventoryItem = m_EquippableInventory[Index];
			if (Item == InventoryItem.ItemDescriptor)
			{
				TmpLocations.Add({Index, Slot, InventoryItem.Quantity});
			}
		}
	}

	return TmpLocations;
}

#if 0
int32 UCInventoryComponent::FindItem(const FCItem& Item, const bool bSearchEquippables /*= false*/) const
{
	int32 Index = INDEX_NONE;
	m_Inventory.Find(Item, Index);

	if (bSearchEquippables)
	{
		for (const ECItemSlot Slot : TEnumRange<ECItemSlot>())
		{
			const int32 Index = static_cast<uint8>(Slot);
			const FCItem& InventoryItem = m_EquippableInventory[Index];
			if (Item == InventoryItem.ItemDescriptor)
			{
				TmpLocations.Add({ Index, Slot, InventoryItem.Quantity });
			}
		}
	}
}
#endif  // 0

bool UCInventoryComponent::MoveItemToInventory(const FCItem& Item, UCInventoryComponent* InventoryReceiver)
{
	if (InventoryReceiver == nullptr)
	{
		return false;
	}

	return InventoryReceiver->AddItem(Item, ECItemSlot::None) && RemoveItem(Item, Item.Quantity);
}

ACItemActor* UCInventoryComponent::CreatePickup(const FCItem& Item, const int32 Quantity)
{
	FTransform Transform;
	if (ACItemActor* Pickup = GetWorld()->SpawnActorDeferred<ACItemActor>(Item.ItemDescriptor->GetPickupClass(), Transform))
	{
		Pickup->SetItemDescriptor(Item.ItemDescriptor);
		Pickup->SetQuantity(Quantity);
		Pickup->SetRarity(Item.Rarity);

		Pickup->FinishSpawning(Transform);

		return Pickup;
	}

	return nullptr;
}
