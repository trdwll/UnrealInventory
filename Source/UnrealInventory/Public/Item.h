// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "Item.generated.h"

class UCItemDescriptorBase;

UCLASS()
class UNREALINVENTORY_API ACItemActor : public AActor
{
	GENERATED_BODY()

public:
	ACItemActor();

	UFUNCTION(BlueprintPure, Category = "UnrealInventory|Item")
	const UCItemDescriptorBase* GetItemDescriptor() const { return m_ItemDescriptor; }
	void SetItemDescriptor(UCItemDescriptorBase* NewDescriptor) { m_ItemDescriptor = NewDescriptor; }

	UFUNCTION(BlueprintPure, Category = "UnrealInventory|Item")
	const int32 GetQuantity() const { return m_Quantity; }
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(BlueprintPure, Category = "UnrealInventory|Item")
	const ECItemRarity GetRarity() const { return m_Rarity; }
	void SetRarity(const ECItemRarity NewRarity);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "UnrealInventory|Item", meta = (DisplayName = "Item Descriptor"))
	UCItemDescriptorBase* m_ItemDescriptor;

private:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetQuantity(const int32 NewQuantity);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRarity(const ECItemRarity NewRarity);

	UPROPERTY(Replicated)
	int32 m_Quantity = 1;

	UPROPERTY(Replicated)
	ECItemRarity m_Rarity = ECItemRarity::Common;
};
