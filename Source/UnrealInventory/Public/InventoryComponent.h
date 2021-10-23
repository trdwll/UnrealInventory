// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemDataAsset.h"

#include <Components/ActorComponent.h>
#include <CoreMinimal.h>

#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (UnrealInventory), meta = (BlueprintSpawnableComponent))
class UNREALINVENTORY_API UCInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "UnrealInventory")
	bool AddItem(const FCItem& Item, const ECItemSlot Slot = ECItemSlot::None);
	bool RemoveItem(const FCItem& Item, const int32 Quantity = 1, const ECItemSlot Slot = ECItemSlot::None);

	UFUNCTION(BlueprintCallable, Category = "UnrealInventory")
	bool DropItem(UPARAM(ref) FCItem& Item, const int32 Quantity = 1);

	/** Trade items from this inventory to another inventory. */
	bool TradeItems(const TArray<FCItem>& ItemsToTrade, UCInventoryComponent* InventoryReceiver);

	bool HasItemInEquippableSlot(const ECItemSlot Slot) const;
	bool CanAddItemToSlot(const UCItemDescriptorBase* Item, const ECItemSlot Slot) const;

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	float GetTotalWeight() const;

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	float GetMaxWeight() const { return m_MaxWeight; }

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	bool GetItemsFromCategory(const ECItemCategory Category, TArray<FCItem>& OutItems);

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	const FCItem& GetItemByIndex(const ECItemSlot Slot = ECItemSlot::None, const int32 Index = -1);

	int32 GetMatchingItemCount(const UCItemDescriptorBase* ItemToFind) const;

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	ECItemSlot GetItemSlot(const FCItem& Item, const bool bSearchEquippables = false) const;

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	const FCItem& GetEquippableItemBySlot(const ECItemSlot Slot) const { return m_EquippableInventory[static_cast<uint8>(Slot)]; }

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	const TArray<FCItem>& GetInventory() const { return m_Inventory; }

	UFUNCTION(BlueprintPure, Category = "UnrealInventory")
	int32 GetItemIndex(const FCItem& Item) const;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnrealInventory|Config", meta = (DisplayName = "Max Weight"))
	float m_MaxWeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "UnrealInventory", meta = (DisplayName = "TestItem"))
	TSoftObjectPtr<UCItemDescriptorBase> m_TestItem;

private:
	struct FCItemLocation
	{
		int32 Index = INDEX_NONE;
		ECItemSlot Slot = ECItemSlot::None;
		int32 Quantity = 0;
	};

	TArray<FCItemLocation> FindItemLocation(const UCItemDescriptorBase* Item, const bool bSearchEquippables = false) const;

	/** Inventory of equippable items - basically itemslots other than None */
	UPROPERTY(Replicated)
	FCItem m_EquippableInventory[ECItemSlot::MAX];

	/** Inventory of items in the slot none. */
	UPROPERTY(Replicated)
	TArray<FCItem> m_Inventory;

	bool AddItemInternal(const FCItem& Item, const ECItemSlot Slot);
	bool RemoveItemInternal(const FCItem& Item, const int32 Quantity, const ECItemSlot Slot);
	bool DropItemInternal(const FCItem& Item, const int32 Quantity, const ECItemSlot Slot);

	bool MoveItemToInventory(const FCItem& Item, UCInventoryComponent* InventoryReceiver);

	ACItemActor* CreatePickup(const FCItem& Item, const int32 Quantity = INDEX_NONE);
};
