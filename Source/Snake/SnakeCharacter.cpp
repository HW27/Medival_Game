/// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SnakeCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "GameplayController.h"
#include "CampFire.h"
#include "MeleeWeapon.h"
#include "GameFramework/SpringArmComponent.h"


//////////////////////////////////////////////////////////////////////////
// ASnakeCharacter

ASnakeCharacter::ASnakeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

												   // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
												   // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
												   //CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
												   //CollectionSphere->SetupAttachment(RootComponent);
												   //CollectionSphere->SetSphereRadius(SphereRadius);
	
	
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->AttachTo(RootComponent);

	//WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	//WeaponMesh->AttachTo(RootComponent);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASnakeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASnakeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASnakeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASnakeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASnakeCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASnakeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASnakeCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASnakeCharacter::OnResetVR);


	PlayerInputComponent->BindAction("CastSpell_1", IE_Pressed, this, &ASnakeCharacter::CastSpell);

}

void ASnakeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FullHealth = 100.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;

	FullMagic = 100.0f;
	Magic = FullMagic;
	MagicPercentage = 1.0f;
	PreviousMagic = MagicPercentage;
	MagicValue = 0.0f;
	bCanUseMagic = true;

	if (MagicCurveCast)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("SetMagicState") });
		MyTimeline.AddInterpFloat(MagicCurveCast, TimelineCallback);
		MyTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}

	ExperienceCap = 1000;
	lvl = 1;
	CurrentExperience = 0;
	Strength=10;
	Agility=10;
	Wisdom = 10;
	Stamina=10;
	SkillPoints = 0;
	ExpPercentage = 0.0f;
	

	ShieldMesh->AttachTo(this->GetMesh(), "Shield_Socket");
	//WeaponMesh->AttachTo(this->GetMesh(), "Weapon_Socket");

	bIsWeaponEquipped = false;
}

void ASnakeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//MANA TIMER
	MyTimeline.TickTimeline(DeltaTime);

	//INVENTORY AND PICKUPS
	CheckForInteractables();

	if (CurrentExperience >= ExperienceCap)
		LevelUp();
	

}


void ASnakeCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASnakeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASnakeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ASnakeCharacter::CastSpell()
{
	GetWorld()->SpawnActor<ACampFire>(GetCharacterMovement()->GetActorLocation(), FRotator(0, 0, 0) );
	UE_LOG(LogTemp, Warning, TEXT("SPELL"));
	MyTimeline.Stop();
	GetWorldTimerManager().ClearTimer(MagicTimerHandle);
	SetMagicChange(-20.0f);
	GetWorldTimerManager().SetTimer(MagicTimerHandle, this, &ASnakeCharacter::UpdateMagic, 5.0f, false);
}

void ASnakeCharacter::OpenSkillTree()
{
	UE_LOG(LogTemp, Warning, TEXT("SKillTree "));
}

void ASnakeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASnakeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASnakeCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASnakeCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

float ASnakeCharacter::GetHealth()
{
	return HealthPercentage;
}

float ASnakeCharacter::GetMagic()
{
	return MagicPercentage;
}

FText ASnakeCharacter::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

FText ASnakeCharacter::GetMagicIntText()
{
	int32 MP = FMath::RoundHalfFromZero(MagicPercentage * 100);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(FullMagic);
	FString MagicHUD = MPS + FString(TEXT("/")) + FullMPS;
	FText MPText = FText::FromString(MagicHUD);
	return MPText;
}

void ASnakeCharacter::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ASnakeCharacter::SetDamageState, 2.0f, false);
}

void ASnakeCharacter::SetDamageState()
{
	bCanBeDamaged = true;
}

void ASnakeCharacter::SetMagicValue()
{
	TimelineValue = MyTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurveCast->GetFloatValue(TimelineValue);
	Magic = FMath::Clamp(CurveFloatValue*FullHealth, 0.0f, FullMagic);
	MagicPercentage = FMath::Clamp(CurveFloatValue, 0.0f, 1.0f);
}

void ASnakeCharacter::SetMagicState()
{
	bCanUseMagic = true;
	MagicValue = 0.0;
}

void ASnakeCharacter::SetMagicChange(float MagicChange)
{
	bCanUseMagic = false;
	PreviousMagic = MagicPercentage;
	MagicValue = (MagicChange / FullMagic);

	MyTimeline.PlayFromStart();
}

void ASnakeCharacter::UpdateMagic()
{
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.0f;
	MyTimeline.PlayFromStart();
}

bool ASnakeCharacter::PlayFlash()
{
	if (redFlash)
	{
		redFlash = false;
		return true;
	}

	return false;
}

void ASnakeCharacter::ReceivePointDamage(float Damage, const UDamageType * DamageType, FVector HitLocation, FVector HitNormal, UPrimitiveComponent * HitComponent, FName BoneName, FVector ShotFromDirection, AController * InstigatedBy, AActor * DamageCauser, const FHitResult & HitInfo)
{
	bCanBeDamaged = false;
	redFlash = true;
	UpdateHealth(-Damage);
	DamageTimer();
}

void ASnakeCharacter::UpdateHealth(float HealthChange)
{

	Health = FMath::Clamp(Health += HealthChange, 0.0f, FullHealth);
	HealthPercentage = Health / FullHealth;
}

void ASnakeCharacter::CheckForInteractables()
{
	FHitResult HitResult;

	FVector StartTrace = FollowCamera->GetComponentLocation();
	FVector EndTrace = (FollowCamera->GetForwardVector() * 500 )+ StartTrace;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	AGameplayController* MyController = Cast<AGameplayController>(GetController());
	if(!MyController)
	{
		return;
	}
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, QueryParams) && MyController)
	{
		//Checks if the item we hit is interab;le
		if (AInteractable* Interactable = Cast<AInteractable>(HitResult.GetActor()))
		{
			MyController->CurrentInteractable = Interactable;
			return;
		}
	}
	MyController->CurrentInteractable = nullptr;
}

void ASnakeCharacter::LevelUp()
{
	lvl+=1;
	CurrentExperience = CurrentExperience-ExperienceCap;
	ExperienceCap += 750;
	ExpPercentage = (float)CurrentExperience / (float)ExperienceCap;
	SkillPoints += 1;
	AddExperiance(0);
	
}

FText ASnakeCharacter::GetLvlValue()
{
	FString HPS = FString::FromInt(lvl);
	FString HealthHUD = HPS + FString(TEXT(" LVL"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;

}

void ASnakeCharacter::AddExperiance(int Value)
{
	//CurrentExperience = FMath::Clamp(CurrentExperience += Value, 0, ExperienceCap);
	CurrentExperience += Value;
	ExpPercentage = (float)CurrentExperience / (float)ExperienceCap;
}

FText ASnakeCharacter::GetExpValue()
{
	FString HPS = FString::FromInt(CurrentExperience);
	FString HPE = FString::FromInt(ExperienceCap);
	FString HealthHUD = HPS + FString(TEXT(" /")) + HPE;
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

FText ASnakeCharacter::GetSkillPointsValue()
{
	FString SKILL = FString::FromInt(SkillPoints);
	FString SkillHUD = FString(TEXT("Skill Points: ") + SKILL);
	FText SKILLText = FText::FromString(SkillHUD);
	return SKILLText;
}

float ASnakeCharacter::GetExperiencePercentage()
{
	return ExpPercentage;
}

int ASnakeCharacter::GetExperianceCap()
{
	return ExperienceCap;
}

int ASnakeCharacter::GetCurrentExperience()
{
	return CurrentExperience;
}

int ASnakeCharacter::GetStrength()
{
	return Strength;
}

int ASnakeCharacter::GetStamina()
{
	return Stamina;
}

int ASnakeCharacter::GetAgility()
{
	return Agility;
}

int ASnakeCharacter::GetWisdom()
{
	return Wisdom;
}

int ASnakeCharacter::GetSkillPoints()
{
	return SkillPoints;
}

void ASnakeCharacter::AddStrength()
{
	Strength+=1;
	SkillPoints-=1;
}

void ASnakeCharacter::AddStamina()
{
	Stamina += 1;
	SkillPoints -= 1;
}

void ASnakeCharacter::AddWisdom()
{
	Wisdom += 1;
	SkillPoints -= 1;
}

void ASnakeCharacter::AddAgility()
{
	Agility += 1;
	SkillPoints -= 1;
}

void ASnakeCharacter::AttachWeaponToCharacter(AMeleeWeapon* weapon)
{
	//weapon->WeaponMesh->AttachTo(this->GetMesh(), "Weapon_Socket");
	weapon->WeaponMesh->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), "Weapon_Socket");
	//WeaponMesh->AttachTo(RootComponent);
	//UE_LOG(LogTemp, Warning, TEXT("ASnakeCharacter::AttachWeaponToCharacter"));
	//WeaponMesh->AttachTo(this->GetMesh(), "Weapon_Socket");
}
