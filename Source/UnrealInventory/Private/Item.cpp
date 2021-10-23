// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

ACItemActor::ACItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}

void ACItemActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACItemActor, m_Quantity);
	DOREPLIFETIME(ACItemActor, m_Rarity);
}

void ACItemActor::SetQuantity(const int32 NewQuantity)
{
	if (!HasAuthority())
	{
		ServerSetQuantity(NewQuantity);
	}

	m_Quantity = NewQuantity;
}

void ACItemActor::SetRarity(const ECItemRarity NewRarity)
{
	if (!HasAuthority())
	{
		ServerSetRarity(NewRarity);
	}

	m_Rarity = NewRarity;
}

void ACItemActor::ServerSetQuantity_Implementation(const int32 NewQuantity)
{
	SetQuantity(NewQuantity);
}

bool ACItemActor::ServerSetQuantity_Validate(const int32 NewQuantity)
{
	return true;
}

void ACItemActor::ServerSetRarity_Implementation(const ECItemRarity NewRarity)
{
	SetRarity(NewRarity);
}

bool ACItemActor::ServerSetRarity_Validate(const ECItemRarity NewRarity)
{
	return true;
}

