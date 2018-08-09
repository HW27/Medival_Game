// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interactable.h"
#include "SnakeGameMode.h"
//#include "SnakeCharacter.h"
#include "GameplayController.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_API AGameplayController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class AInteractable* CurrentInteractable;


	UFUNCTION(BlueprintCallable, Category = "Utils")
	void CraftItem(FInventoryItem IteamA, FInventoryItem ItemB);
	
	void Interact();

	UFUNCTION(BlueprintCallable, Category="Utils")
	void AddItemToInventoryByID(FName ID);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utils")
		TArray<FInventoryItem> Inventory;

	UFUNCTION(BlueprintImplementableEvent)
		void ReloadMyInventory();

	



protected:
	virtual void SetupInputComponent() override;
};
