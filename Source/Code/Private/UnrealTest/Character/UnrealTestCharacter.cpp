// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealTest/Character/UnrealTestCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

#include "UnrealTest/Game/UT_DeathMatchGameMode.h"
#include "UnrealTest/Character/UT_PlayerState.h"

#include "UnrealTest/Items/Door.h"

AUnrealTestCharacter::AUnrealTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = TURN_RATE_GAMEPAD;

	DisableControllerRotation();

	ConfigureCharacterMovement(GetCharacterMovement());
	
	SetCameraBoom();
	SetFollowCamera();
	
	bReplicates = true;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AUnrealTestCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AUnrealTestCharacter::OnOverlapEnd);

	CurrentDoor = nullptr;
}

void AUnrealTestCharacter::DisableControllerRotation()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AUnrealTestCharacter::ConfigureCharacterMovement(UCharacterMovementComponent* characterMovement)
{
	// Configure character movement
	characterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	characterMovement->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	characterMovement->JumpZVelocity = JUMP_Z_VELOCITY;
	characterMovement->AirControl = AIR_CONTROL;
	characterMovement->MaxWalkSpeed = MAX_WALK_SPEED;
	characterMovement->MinAnalogWalkSpeed = MIN_ANALOG_WALK_SPEED;
	characterMovement->BrakingDecelerationWalking = BRAKING_DECELERATION_WALKING;
}

void AUnrealTestCharacter::SetCameraBoom()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
}

void AUnrealTestCharacter::SetFollowCamera()
{
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AUnrealTestCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && OtherActor->GetClass()->IsChildOf(ADoor::StaticClass()))
	{
		CurrentDoor = Cast<ADoor>(OtherActor);
	}
}

void AUnrealTestCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		CurrentDoor = nullptr;
	}
}

void AUnrealTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnrealTestCharacter, CurrentDoor);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	JumpBinding(PlayerInputComponent);

	MovementBinding(PlayerInputComponent);

	TurnBinding(PlayerInputComponent);
	LookUpBinding(PlayerInputComponent);
	
	TouchBinding(PlayerInputComponent);

	ActionBinding(PlayerInputComponent);
}

void AUnrealTestCharacter::JumpBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AUnrealTestCharacter::MovementBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AUnrealTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AUnrealTestCharacter::MoveRight);
}

void AUnrealTestCharacter::TurnBinding(class UInputComponent* PlayerInputComponent)
{
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AUnrealTestCharacter::TurnAtRate);
}

void AUnrealTestCharacter::LookUpBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AUnrealTestCharacter::LookUpAtRate);
}

void AUnrealTestCharacter::TouchBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AUnrealTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AUnrealTestCharacter::TouchStopped);
}

void AUnrealTestCharacter::ActionBinding(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AUnrealTestCharacter::OnAction);
}

void AUnrealTestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AUnrealTestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AUnrealTestCharacter::OnAction()
{
	if (CurrentDoor)
	{
		const FVector ForwardVector = FollowCamera->GetForwardVector();
		if (HasAuthority())
		{
			CurrentDoor->ToggleDoor(ForwardVector);
		}
		else
		{
			Server_OnAction();
		}
	}
}

void AUnrealTestCharacter::Server_OnAction_Implementation()
{
	OnAction();
}

bool AUnrealTestCharacter::Server_OnAction_Validate()
{
	return true;
}

void AUnrealTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUnrealTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUnrealTestCharacter::PossessedBy(AController* C)
{
	Super::PossessedBy(C);

	//Update Colors in server
	if (AUT_PlayerState* playerState = Cast<AUT_PlayerState>(GetPlayerState()))
	{
		UpdateTeamColors(playerState->GetTeamNum());
	}
}

void AUnrealTestCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// As soon as PlayerState is assigned, set team colors for local player
	if (GetPlayerState() != NULL)
	{
		if (AUT_PlayerState* playerState = Cast<AUT_PlayerState>(GetPlayerState()))
		{
			UpdateTeamColors(playerState->GetTeamNum());
		}
	}
}

void AUnrealTestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AUnrealTestCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
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

void AUnrealTestCharacter::Multicast_ApplyRagdoll_Implementation()
{
	if (!GetMesh())
	{
		return;
	}

	//Set Collison Preset to Ragdoll
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	//Ragdoll
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->AddImpulseAtLocation(GetActorForwardVector() * -1000, GetActorLocation());
}

void AUnrealTestCharacter::Multicast_ReAttachRagdoll_Implementation()
{
	if (!GetMesh())
	{
		return;
	}

	GetMesh()->SetSimulatePhysics(false);

	//Default Relative Loc of the player in viewport
	FVector relativeLoc = GetCapsuleComponent()->GetComponentLocation();
	relativeLoc.Z -= 90.f;

	//Default Relative Rot the player in viewport
	FQuat4d relativeRot = GetCapsuleComponent()->GetComponentQuat();
	FQuat4d rotationAmount = FQuat4d::MakeFromEuler(FVector3d(0, 0, 270));

	GetMesh()->SetRelativeLocationAndRotation(relativeLoc, (relativeRot * rotationAmount));
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
}

void AUnrealTestCharacter::Multicast_PlayAnimation_Implementation(UAnimMontage* MontageToPlay)
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}
}

void AUnrealTestCharacter::EnableMovement() const
{
	if (UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		CharacterComp->SetDefaultMovementMode();
	}
}

void AUnrealTestCharacter::DisableMovement() const
{
	if (UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
	}
}

void AUnrealTestCharacter::EnablePlayerInput()
{
	if (GetWorld())
	{
		if (APlayerController* playerCont = GetWorld()->GetFirstPlayerController())
		{
			EnableInput(playerCont);
		}
	}
}

void AUnrealTestCharacter::DisablePlayerInput()
{
	if (GetWorld())
	{
		if (APlayerController* playerCont = GetWorld()->GetFirstPlayerController())
		{
			DisableInput(playerCont);
		}
	}
}

int32 AUnrealTestCharacter::GetPlayerTeam() const
{
	if (AUT_PlayerState* playerState = Cast<AUT_PlayerState>(GetPlayerState()))
	{
		return playerState->GetTeamNum();
	}
	return -1;
}

void AUnrealTestCharacter::EnableCapsuleCollision()
{
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AUnrealTestCharacter::DisableCapsuleCollision() const
{
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

