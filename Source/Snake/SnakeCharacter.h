// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "SnakeCharacter.generated.h"

USTRUCT(BlueprintType)
struct FCraftingInfo : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ComponentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ProductID;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDestroyItemA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDestroyItemB;
};


USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		FInventoryItem()
		{
			Name = FText::FromString("Item");
			Action = FText::FromString("Use");
			Description = FText::FromString("Please enter a description for this item");
			Value = 10;
		}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class APickup> ItemPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanBeUsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCraftingInfo> CraftCombinations;

	
	bool operator==(const FInventoryItem& Item) const
	{
		if (ItemID == Item.ItemID)
			return true;
		else return false;
	}
};


UCLASS(config = Game)
class ASnakeCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ASnakeCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void CastSpell();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////HEALTH///////////////////////////////////////////
	//////////////////////////////////////////AND/////////////////////////////////////////////
	//////////////////////////////////////////MANA////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float FullMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float Magic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float MagicPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float PreviousMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float MagicValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool redFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		UCurveFloat *MagicCurveCast;


	UPROPERTY(EditAnywhere, Category = "Magic")
		FTimeline MyTimeline;

	UPROPERTY(EditAnywhere, Category = "Magic")
		FTimerHandle MemberTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Magic")
		FTimerHandle MagicTimerHandle;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanUseMagic;

	/** Get Health */
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealth();

	/** Get Magic */
	UFUNCTION(BlueprintPure, Category = "Magic")
		float GetMagic();

	/** Get Health Text */
	UFUNCTION(BlueprintPure, Category = "Health")
		FText GetHealthIntText();

	/** Get Magic Text */
	UFUNCTION(BlueprintPure, Category = "Magic")
		FText GetMagicIntText();

	/** Damage Timer */
	UFUNCTION()
		void DamageTimer();

	/** Set Damage State */
	UFUNCTION()
		void SetDamageState();

	/** Set Magic Value */
	UFUNCTION()
		void SetMagicValue();

	/** Set Damage State */
	UFUNCTION()
		void SetMagicState();

	/** Set Damage State */
	UFUNCTION()
		void SetMagicChange(float MagicChange);

	/** Set Damage State */
	UFUNCTION()
		void UpdateMagic();

	/** Play Flash */
	UFUNCTION(BlueprintPure, Category = "Health")
		bool PlayFlash();

	UFUNCTION()
		void ReceivePointDamage(float Damage, const UDamageType * DamageType, FVector HitLocation, FVector HitNormal, UPrimitiveComponent * HitComponent, FName BoneName, FVector ShotFromDirection, AController * InstigatedBy, AActor * DamageCauser, const FHitResult & HitInfo);

	UFUNCTION(BlueprintCallable, Category = "Power")
		void UpdateHealth(float HealthChange);

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////INVENTORY//////////////////////////////////////////
	//////////////////////////////////////////AND/////////////////////////////////////////////
	///////////////////////////////////////CRAFTSYSTEM////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////

	void CheckForInteractables();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int lvl;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int CurrentExperience;

	UFUNCTION()
		void LevelUp();

	UFUNCTION(BlueprintPure, Category = "Level")
		FText GetLvlValue();

	UPROPERTY()
		int ExperienceCap;

	UFUNCTION(BlueprintCallable, Category = "Level")
		void AddExperiance(int Value);

	UFUNCTION(BlueprintPure, Category = "Level")
		FText GetExpValue();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float ExpPercentage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Strength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Agility;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Wisdom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Stamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int SkillPoints;

	UFUNCTION(BlueprintPure, Category = "Level")
		FText GetSkillPointsValue();

	UFUNCTION(BlueprintPure, Category = "Level")
		float GetExperiencePercentage();

	UFUNCTION(BlueprintPure, Category = "Level")
		int GetExperianceCap();

	UFUNCTION(BlueprintPure, Category = "Level")
		int GetCurrentExperience();

};

