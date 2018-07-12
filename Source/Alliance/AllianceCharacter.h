// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "AllianceCharacter.generated.h"

UENUM(BlueprintType)		//State of the character (Maybe some character does not use all of them)
enum class EState : uint8
{
	S_Running	UMETA(DisplayName = "Running"),
	S_Idle		UMETA(DisplayName = "Idle"),
	S_Blocking	UMETA(DisplayName = "Blocking"),
	S_JumpAttacking UMETA(DisplayName = "JumpAttacking"),
	S_Attacking UMETA(DisplayName = "Attacking"),
	S_SpecialAttacking UMETA(DisplayName = "SpecialAttacking")
};

UCLASS(config=Game)
class AAllianceCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AAllianceCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	// Called when player attacks
	void PrimaryAttack();

	// Called when player sprints
	void StartSprint();
	void StopSprint();

	// Called when player blocks
	void StartBlock();
	void StopBlock();

	// Called to reset speed after attacking
	UFUNCTION(BlueprintCallable)
	void ResetMoveSpeed();

	// Called when player interact to start the minigame
	void Interact();

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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// PROPERTIES

	// State of the character
	UPROPERTY(BlueprintReadWrite, Category = MyCharacter)
	EState CurrentState;
	
	// Variables used in blueprints for animations
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = MyCharacter)
		bool b_ChainAttack;
	UPROPERTY(BlueprintReadWrite, Category = MyCharacter)
		bool b_IsEvading;
	UPROPERTY(BlueprintReadWrite, Category = MyCharacter)
		float Sprint;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = MyCharacter)
		float LaunchForce;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = MyCharacter)
		float LaunchHeight;
	UPROPERTY(BlueprintReadWrite, Category = MyCharacter)
		float DamageMultiplier;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = MyCharacter)
		int Combo;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = MyCharacter)
		bool InMinigame;
	
	// Variables to control player's health and stamine
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat")
		float Health;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat")
		float Stamina;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat")
		bool b_IsDead;


	// Variables for controlling Damage
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
		float Primary_Attack_Dmg;
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
		float Secondary_Attack_Dmg;

	UPROPERTY(BlueprintReadOnly)
		bool b_IAmServer;

	// Delegate executed when the player attacks
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEvent);
	UPROPERTY(BlueprintAssignable)
		FOnAttackEvent OnAttackEvent;

	// Delegate executed when the player starts the minigame
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartMinigame);
	UPROPERTY(BlueprintAssignable)
		FStartMinigame OnStartMinigame;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovePiece);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
		FMovePiece OnMovePiece;

	// FUNCTIONS

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// This function remove the movement (axis and actions mappings) of the character
	UFUNCTION(BlueprintCallable)
		void RemoveCharacterMovementBindings();

	// This function returns the movement (axis and actions mappings) to the character
	void SetCharacterMovement(class UInputComponent* InputComponent);

	FORCEINLINE UInputComponent* GetCharacterInputComponent() { return CharacterMovementInputComponent.Get(); }

	// This function is executed only by the server. It reduces the amount of player's health
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	// This function applies damage other actor
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void DoDmg(AActor* DamagedActor, float Dmg) const;
	
	// RPC function called when the player dies
	UFUNCTION(Reliable, NetMulticast)
		void ExecuteWhenDead();
	void ExecuteWhenDead_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerClientAttacking();
	void OnServerClientAttacking_Implementation();
	FORCEINLINE bool OnServerClientAttacking_Validate() { return true; }

	UFUNCTION(Reliable, NetMulticast)
		void StartSprinting();
	void StartSprinting_Implementation();

	UFUNCTION(Reliable, NetMulticast)
		void StopSprinting();
	void StopSprinting_Implementation();

	UFUNCTION(Reliable, NetMulticast)
		void StartBlocking();
	void StartBlocking_Implementation();

	UFUNCTION(Reliable, NetMulticast)
		void StopBlocking();
	void StopBlocking_Implementation();

	UFUNCTION(Reliable, NetMulticast)
		void ResetSpeed();
	void ResetSpeed_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerClientStartSprinting();
	void OnServerClientStartSprinting_Implementation();
	FORCEINLINE bool OnServerClientStartSprinting_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerClientStopSprinting();
	void OnServerClientStopSprinting_Implementation();
	FORCEINLINE bool OnServerClientStopSprinting_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerClientStartBlocking();
	void OnServerClientStartBlocking_Implementation();
	FORCEINLINE bool OnServerClientStartBlocking_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerClientStopBlocking();
	void OnServerClientStopBlocking_Implementation();
	FORCEINLINE bool OnServerClientStopBlocking_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerClientResetSpeed();
	void OnServerClientResetSpeed_Implementation();
	FORCEINLINE bool OnServerClientResetSpeed_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
		void OnServerStartMinigame();
	void OnServerStartMinigame_Implementation();
	FORCEINLINE bool OnServerStartMinigame_Validate() { return true; }

private:

	// UInputComponent pointer used to add or remove the action and axis mappings
	TWeakObjectPtr<class UInputComponent> CharacterMovementInputComponent;

};

