// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataAsset.h"

const FCItemRarityData& UCItemDescriptorBase::GetRarityData(const ECItemRarity Rarity) const
{
	return m_Rarity[static_cast<uint8>(Rarity)];
}

TSubclassOf<ACItemActor> UCItemDescriptorBase::GetPickupClass() const
{
	if (m_PickupClass.IsPending())
	{
		return m_PickupClass.LoadSynchronous();
	}

	return m_PickupClass.Get();
}
