// Fill out your copyright notice in the Description page of Project Settings.

#include "CampFire.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACampFire::ACampFire()
{
	MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My BOX COMPONENT"));
	MyBoxComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent = MyBoxComponent;

	Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("My Fire"));
	//Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("/Game/StarterConent/Particles/P_Fire"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireParticle(TEXT("/Game/StarterContent/Particles/P_Fire"));
	
	if (FireParticle.Object != NULL)
	{
		Fire->SetTemplate(FireParticle.Object);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("FIrePArticle not found"));

	Fire->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Fire->SetupAttachment(RootComponent);

	MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACampFire::OnOverlapBegin);
	MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACampFire::OnOverlapEnd);

	bCanApplyDamage = false;

}

void ACampFire::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap-Camp"));
		bCanApplyDamage = true;
		MyCharacter = Cast<AActor>(OtherActor);
		MyHit = SweepResult;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFire::ApplyFireDamage, 2.2f, true, 0.0f);

	}
}
void  ACampFire::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanApplyDamage = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ACampFire::ApplyFireDamage()
{
	if (bCanApplyDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACampFire::ApplyFireDamage()___1"));
		UGameplayStatics::ApplyPointDamage(MyCharacter, 20.0f, GetActorLocation(), MyHit, nullptr, this, FireDamageType);

	}
}