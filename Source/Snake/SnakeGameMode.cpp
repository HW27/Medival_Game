// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SnakeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"



ASnakeGameMode::ASnakeGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	HUDClass = AMyHUD::StaticClass();
	//MyCharacter = Cast<ASnakeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	
}

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<ASnakeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void ASnakeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}

	}
}

EGamePlayState ASnakeGameMode::GetCurrentState() const
{
	return CurrentState;
}

void ASnakeGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void ASnakeGameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
	{
		// do nothing
	}
	break;
	// Unknown/default state
	case EGamePlayState::EGameOver:
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
	break;
	// Unknown/default state
	default:
	case EGamePlayState::EUnknown:
	{
		// do nothing
	}
	break;
	}
}

