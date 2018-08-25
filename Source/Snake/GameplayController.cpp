// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayController.h"
//#include "Interactable.h"
//#include "SnakeGameMode.h"
//#include "SnakeCharacter.h"


void AGameplayController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Use", IE_Pressed, this, &AGameplayController::Interact);
}

void AGameplayController::CraftItem(FInventoryItem ItemA, FInventoryItem ItemB)
{
	for (auto Craft : ItemB.CraftCombinations)
	{
		if (Craft.ComponentID == ItemA.ItemID)
		{
			if (Craft.bDestroyItemA)
			{
				Inventory.RemoveSingle(ItemA);
			}
			if (Craft.bDestroyItemB)
			{
				Inventory.RemoveSingle(ItemB);
			}

			AddItemToInventoryByID(Craft.ProductID); 
			ReloadMyInventory();
		}
	}
}

void AGameplayController::Interact()
{
	if (CurrentInteractable)
	{
		CurrentInteractable->Interact(this);
	}
}

void AGameplayController::AddItemToInventoryByID(FName ID)
{
	ASnakeGameMode* GameMode = Cast<ASnakeGameMode>(GetWorld()->GetAuthGameMode());
	UDataTable* ItemTable = GameMode->GetItemDB();

	FInventoryItem* ItemToAdd = ItemTable->FindRow<FInventoryItem>(ID, "");

	if (ItemToAdd)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemtoAdd"));
		Inventory.Add(*ItemToAdd);
	}
}

