// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SnakeCharacter.h"
#include "Engine.h"
#include "MyHUD.h"
#include "SnakeGameMode.generated.h"

//enum to store the current state of gameplay
UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class ASnakeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASnakeGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Health")
		EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

private:
	/**Keeps track of the current playing state */
	EGamePlayState CurrentState;

	/**Handle any function calls that rely upon changing the playing state of our game */
	void HandleNewState(EGamePlayState NewState);

	UPROPERTY(VisibleAnywhere)
		ASnakeCharacter* MyCharacter;


public:

	UPROPERTY(EditDefaultsOnly)
		class UDataTable* ItemBD;

	//UFUNCTION(VisibleAnywhere)
	UDataTable* GetItemDB() const { return ItemBD; }


};