// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryConstants.h"

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>

#include "ItemDataAsset.generated.h"

class UTexture2D;
class ACItemActor;

UENUM(BlueprintType)
enum class ECItemRarity : uint8
{
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECItemSlot : uint8
{
	None,  // If the item is in the inventory rather than equipped
	Head,
	Chest,
	Hands,
	Pants,
	Shoes,
	Shield,

	Special1,  // Such as arrows
	Special2,  // Such as bullets

	Passive1,  // Such as ring
	Passive2,  // Such as necklass
	Passive3,  // Such as earrings

	Bag1,
	Bag2,
	Bag3,

	Weapon1,
	Weapon2,

	Usable1,
	Usable2,
	Usable3,
	Usable4,

	Tool1,
	Tool2,
	Tool3,
	Tool4,
	Tool5,
	Tool6,

	MAX UMETA(Hidden),
	Invalid
};

ENUM_RANGE_BY_FIRST_AND_LAST(ECItemSlot, ECItemSlot::Head, ECItemSlot::Tool6)

UENUM(BlueprintType)
enum class ECItemCategory : uint8
{
	Weapons,
	Apparel,
	Fishing,
	Tools,
	Utilities,
	Cooking,
	Resources,
	Quest,
	Ammo
};

UENUM(BlueprintType)
enum class ECItemHealthGroup : uint8
{
	OneQuarter UMETA(DisplayName = "1/4 - 0%-25%"),
	TwoQuarter UMETA(DisplayName = "2/4 - 26%-50%"),
	ThreeQuarter UMETA(DisplayName = "3/4 - 51%-75%"),
	Full UMETA(DisplayName = "4/4 - 76%-100%"),

	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCItemRarityData
{
	GENERATED_BODY()

	/** The gear score range for this item. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Score Range"))
	FIntPoint ScoreRange;

	/** How much does the item weight? */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Weight"))
	float Weight = 0.1f;

	/** How much currency is this item worth based on the health? */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Value"))
	float Value[ECItemHealthGroup::MAX];

	// #TRDWLL: Can add an array of perks or stats here for each rarity
};

UCLASS(BlueprintType)
class UNREALINVENTORY_API UCItemDescriptorBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	const FText& GetTitle() const { return m_Title; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	const FText& GetDescription() const { return m_Description; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	UTexture2D* GetIcon() const { return m_Icon; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	int32 GetStackSize() const { return m_StackSize; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	int32 GetItemSlot() const { return m_ItemSlot; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	ECItemCategory GetItemCategory() const { return m_ItemCategory; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Item")
	const FCItemRarityData& GetRarityData(const ECItemRarity Rarity) const;

	TSubclassOf<ACItemActor> GetPickupClass() const;

protected:
	/** The name of this item. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Title"))
	FText m_Title;

	/** A brief description for the item. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Description"))
	FText m_Description;

	/** The icon of the item that's displayed in the inventory. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Icon"))
	UTexture2D* m_Icon = nullptr;

	/** How many items of this can be stacked? */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Max Stack Size"))
	int32 m_StackSize = 1;

	/** The slot that the item can go into. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Item Slot"), meta = (Bitmask, BitmaskEnum = "ECItemSlot"))
	int32 m_ItemSlot = 0;

	/** The category that the item will be displayed under. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Item Category"))
	ECItemCategory m_ItemCategory = ECItemCategory::Ammo;

	/** Rarity data for the item. */
	UPROPERTY(EditDefaultsOnly, Category = "Item|Config", meta = (DisplayName = "Rarity"))
	FCItemRarityData m_Rarity[ECItemRarity::MAX];

	/** The actor that's spawned into the world. */
	UPROPERTY(EditDefaultsOnly, Category = "World|Config", meta = (DisplayName = "Pickup Class"))
	TSoftClassPtr<ACItemActor> m_PickupClass;

private:
};

UCLASS(BlueprintType)
class UNREALINVENTORY_API UCItemDescriptor : public UCItemDescriptorBase
{
	GENERATED_BODY()

public:
protected:
private:
};

/**
 * The actual item that exists in the players inventory.
 */
USTRUCT(BlueprintType)
struct FCItem
{
	GENERATED_BODY()

	/** The data asset for this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Item Descriptor"))
	UCItemDescriptorBase* ItemDescriptor = nullptr;

	/** How many of this item exist in this slot? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Quantity"))
	int32 Quantity = 1;

	/** How much health does this item in the slot have? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Health"))
	float Health = -1.0f;

	/** The current score of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Score"))
	int32 Score = INDEX_NONE;

	/** The rarity of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (DisplayName = "Rarity"))
	ECItemRarity Rarity = ECItemRarity::Common;

	FCItem() = default;
	FCItem(FCItem&& Other) = default;
	FCItem& operator=(FCItem&& Other) = default;
	FCItem(const FCItem& Other) = default;
	FCItem& operator=(const FCItem& Other) = default;

	bool operator==(const FCItem& Other) const { return ItemDescriptor == Other.ItemDescriptor && Rarity == Other.Rarity; }
	bool operator!=(const FCItem& Other) const { return !(*this == Other); }

	bool IsIdentical(const FCItem& Other) const
	{
		return ItemDescriptor == Other.ItemDescriptor && Quantity == Other.Quantity && Health == Other.Health && Score == Other.Score && Rarity == Other.Rarity;
	}

	// #TRDWLL: make some additional constructors such as (asset, actor), (asset, actor, quantity), etc

	void SetQuantity(const int32 NewQuantity)
	{
		if (NewQuantity <= ItemDescriptor->GetStackSize())
		{
			Quantity = NewQuantity;
		}
	}

	const FCItemRarityData& GetRarityData()
	{
		return ItemDescriptor->GetRarityData(Rarity);
	}

	const float GetTotalWeight() const
	{
		return ItemDescriptor->GetRarityData(Rarity).Weight * Quantity;
	}

	void GenerateScore()
	{
		if (Score == INDEX_NONE)
		{
			const FIntPoint& DefaultScore = ItemDescriptor->GetRarityData(Rarity).ScoreRange;
			Score = FMath::RandRange(DefaultScore.X, DefaultScore.Y);
		}
	}

	bool IsItemValid() const
	{
		return ItemDescriptor != nullptr && Quantity >= 1;
	}

	void Reset()
	{
		ItemDescriptor = nullptr;
		Quantity = 0;
		Health = -1.0f;
		Score = INDEX_NONE;
		Rarity = ECItemRarity::Common;
	}

	/*
	FCItem(const FCItem& Other)
	{
		*this = Other;
	}
	FCItem& operator=(const FCItem& Other)
	{
		Item = Other.Item;
		Quantity = Other.Quantity;
		return *this;
	}
	FCItem(FCItem&& Other)
	{
		*this = MoveTemp(Other);
	}
	FCItem& operator=(FCItem&& Other)
	{
		FMemory::Memswap(this, &Other, sizeof(FCItem));
		return *this;
	}
	*/
};

UCLASS()
class UCItemStatics : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "UnrealInventory|ItemStatics")
	static bool IsItemValid(const FCItem& Item)
	{
		return Item.IsItemValid();
	}

	UFUNCTION(BlueprintPure, Category = "UnrealInventory|ItemStatics")
	static FLinearColor GetRarityColor(const ECItemRarity Rarity)
	{
		switch (Rarity)
		{
		default:
		case ECItemRarity::Common: return UnrealInventory::Items::CommonColor;
		case ECItemRarity::Uncommon: return UnrealInventory::Items::UncommonColor;
		case ECItemRarity::Rare: return UnrealInventory::Items::RareColor;
		case ECItemRarity::Epic: return UnrealInventory::Items::EpicColor;
		case ECItemRarity::Legendary: return UnrealInventory::Items::LegendaryColor;
		}
	}

protected:
private:
};
