// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "SnakeCharacter.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_API ASword : public AInteractable
{
	GENERATED_BODY()
	
public:

	ASword();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Damage;

	UPROPERTY(VisibleAnywhere)
		ASnakeCharacter* MyCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int LevelCap;

	UFUNCTION(BlueprintCallable, Category = "Level")
		void AttachtoCharacter(ASnakeCharacter* Player);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pickup")
		void OnUsed();

	//UPROPERTY(EditAnywhere)
		//UStaticMeshComponent * PickupMesh;
	
};
