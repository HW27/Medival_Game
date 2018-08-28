// Fill out your copyright notice in the Description page of Project Settings.

#include "Shield.h"


// Sets default values
AShield::AShield()
{
	OnActorBeginOverlap.AddDynamic(this, &AShield::OnOverlap);

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));

}

void AShield::OnOverlap(AActor * MyOverlappedActor, AActor * OtherActor)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		MyCharacter = Cast<ASnakeCharacter>(OtherActor);

		if (MyCharacter && MyCharacter->GetEquipShieldState() == false)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AMeleeWeapon::OnOverlap()"));
			MyCharacter->AttachShieldToCharacter(this);
			//Destroy();
		}
	}
}
