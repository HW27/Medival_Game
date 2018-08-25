// Fill out your copyright notice in the Description page of Project Settings.

#include "Sword.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"



ASword::ASword()
{
	MyCharacter = Cast<ASnakeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->AttachTo(RootComponent);
	
	ItemID = FName("Enter ID");


//	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
//	RootComponent = Cast<USceneComponent>(PickupMesh);
}

void ASword::AttachtoCharacter(ASnakeCharacter* Player)
{
	WeaponMesh->AttachTo(Player->GetMesh(), "Weapon_Socket");
}

