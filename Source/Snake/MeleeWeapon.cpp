// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeWeapon.h"


// Sets default values
AMeleeWeapon::AMeleeWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	OnActorBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnOverlap);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	//WeaponMesh->AttachTo(RootComponent);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	//	RootComponent = Cast<USceneComponent>(PickupMesh);
}

void AMeleeWeapon::OnOverlap(AActor * MyOverlappedActor, AActor * OtherActor)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		MyCharacter = Cast<ASnakeCharacter>(OtherActor);
		 
		if (MyCharacter && MyCharacter->GetEquipWeaponState() == false)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AMeleeWeapon::OnOverlap()"));
			MyCharacter->AttachWeaponToCharacter(this);
			//Destroy();
		}
	}
}
