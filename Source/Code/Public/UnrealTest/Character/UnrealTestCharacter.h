// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealTestCharacter.generated.h"

class ADoor;

UCLASS(config=Game)
class AUnrealTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AUnrealTestCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void DisableControllerRotation();
	void ConfigureCharacterMovement(class UCharacterMovementComponent* characterMovement);
	void SetCameraBoom();
	void SetFollowCamera();

	void JumpBinding(class UInputComponent* PlayerInputComponent);
	void MovementBinding(class UInputComponent* PlayerInputComponent);
	void TurnBinding(class UInputComponent* PlayerInputComponent);
	void LookUpBinding(class UInputComponent* PlayerInputComponent);
	void TouchBinding(class UInputComponent* PlayerInputComponent);

	void ActionBinding(class UInputComponent* PlayerInputComponent);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimation(UAnimMontage* MontageToPlay);
	void Multicast_PlayAnimation_Implementation(UAnimMontage* MontageToPlay);
	
	
	/** */
	//TEAM
	// Update the team color of all player meshes.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateTeamColors(int32 TeamColor);

	int32 GetPlayerTeam() const;

protected:
	virtual void PossessedBy(class AController* C) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void OnRep_PlayerState() override;

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

	void OnAction();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnAction();

private:
	/** */
	// Functions tp Enable/Disable Capsule collision
	void EnableCapsuleCollision();
	void DisableCapsuleCollision() const;

	/** */
	// Functions to Enable/Disable movement
	void EnableMovement() const;
	void DisableMovement() const;

	/** */
	//Functions to Enable/Disable Input
	void EnablePlayerInput();
	void DisablePlayerInput();

	/** */
	// Do Ragdoll Anim Server and Clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyRagdoll();
	void Multicast_ApplyRagdoll_Implementation();

	/** */
	// Reattach Ragdoll to capsule
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReAttachRagdoll();
	void Multicast_ReAttachRagdoll_Implementation();

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(Replicated)
	ADoor* CurrentDoor;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input, meta = (AllowPrivateAccess = "true"))
	float TurnRateGamepad;
	
	const float TURN_RATE_GAMEPAD = 50.f;
	const float JUMP_Z_VELOCITY= 700.f;
	const float AIR_CONTROL = 0.35f;
	const float MAX_WALK_SPEED = 500.f;
	const float MIN_ANALOG_WALK_SPEED = 20.f;
	const float BRAKING_DECELERATION_WALKING = 2000.f;
};

