// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"


APickup::APickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	//PickupMesh->SetupAttachment(RootComponent);
	
	ItemID = FName("Enter ID");
}

