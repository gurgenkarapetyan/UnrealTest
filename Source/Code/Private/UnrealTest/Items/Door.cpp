// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealTest/Items/Door.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->InitBoxExtent(FVector(150, 100, 100));
	BoxComponent->SetCollisionProfileName("Trigger");
	RootComponent = BoxComponent;
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	DoorMesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorAsset(TEXT("/Game/StarterContent/Props/SM_Door.SM_Door"));
	if (DoorAsset.Succeeded())
	{
		DoorMesh->SetStaticMesh(DoorAsset.Object);
		DoorMesh->SetRelativeLocation(FVector(0.f, 50.f, -100.0f));
		DoorMesh->SetWorldScale3D(FVector(1.f));
	}

	bIsDoorOpened = false;
	bIsDoorOpening = false;
	bIsDoorClosing = false;

	DotProduct = 0.f;
	MaxDegree = 0.f;
	PositiveNegative = 0.f;
	DoorCurrentRotation = 0.f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), BoxComponent->GetScaledBoxExtent(), FQuat(GetActorRotation()), FColor::Turquoise, true, -1, 0, 2);
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, bIsDoorOpened);
	DOREPLIFETIME(ADoor, bIsDoorOpening);
	DOREPLIFETIME(ADoor, bIsDoorClosing);
	DOREPLIFETIME(ADoor, DotProduct);
	DOREPLIFETIME(ADoor, MaxDegree);
	DOREPLIFETIME(ADoor, AddRotation);
	DOREPLIFETIME(ADoor, PositiveNegative);
	DOREPLIFETIME(ADoor, DoorCurrentRotation);
	DOREPLIFETIME(ADoor, DoorForwardVector);
}

void ADoor::OnRep_DoorToggled()
{
	DotProduct = FVector::DotProduct(BoxComponent->GetForwardVector(), DoorForwardVector);
	PositiveNegative = FMath::Sign(DotProduct);

	MaxDegree = PositiveNegative * 90.f;

	if (bIsDoorOpened)
	{
		bIsDoorOpening = false;
		bIsDoorClosing = true;
		CloseDoor(1.0f);
	}
	else
	{
		bIsDoorClosing = false;
		bIsDoorOpening = true;
		OpenDoor(1.0f);
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::OpenDoor(const float DeltaTime)
{
	DoorCurrentRotation = DoorMesh->GetRelativeRotation().Yaw;

	AddRotation = PositiveNegative * DeltaTime * 80;

	if (FMath::IsNearlyEqual(DoorCurrentRotation, MaxDegree, 1.5f))
	{
		bIsDoorOpening = false;
		bIsDoorClosing = false;
	}
	else if(bIsDoorOpening)
	{
		const FRotator NewRotation = FRotator(0.f, AddRotation, 0.f);
		DoorMesh->AddRelativeRotation(FQuat(NewRotation), false, 0, ETeleportType::None);
	}
}

void ADoor::CloseDoor(const float DeltaTime)
{
	DoorCurrentRotation = DoorMesh->GetRelativeRotation().Yaw;

	if (DoorCurrentRotation > 0)
	{
		AddRotation = -(DeltaTime * 80);	
	}
	else
	{
		AddRotation = DeltaTime * 80;
	}

	if (FMath::IsNearlyEqual(DoorCurrentRotation, 0.f, 1.5f))
	{
		bIsDoorClosing = false;
		bIsDoorOpening = false;
	}
	else if (bIsDoorClosing)
	{
		const FRotator NewRotation = FRotator(0.f, AddRotation, 0.f);
		DoorMesh->AddRelativeRotation(FQuat(NewRotation), false, 0, ETeleportType::None);
	}
}

void ADoor::ToggleDoor(FVector ForwardVector)
{
	DoorForwardVector = ForwardVector;
	if (HasAuthority())
	{
		bIsDoorOpened = !bIsDoorOpened;
		OnRep_DoorToggled();
	}
}
