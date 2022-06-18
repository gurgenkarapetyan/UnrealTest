// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class UNREALTEST_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ToggleDoor(FVector ForwardVector);

	UFUNCTION()
	void OpenDoor(const float DeltaTime);

	UFUNCTION()
	void CloseDoor(const float DeltaTime);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnRep_DoorToggled();
	
private:
	UPROPERTY(EditAnywhere, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(ReplicatedUsing = OnRep_DoorToggled)
	bool bIsDoorOpened;

	UPROPERTY(Replicated)
	bool bIsDoorOpening;
	
	UPROPERTY(Replicated)
	bool bIsDoorClosing;

	UPROPERTY(Replicated)
	float DotProduct;
	
	UPROPERTY(Replicated)
	float MaxDegree;
	
	UPROPERTY(Replicated)
	float AddRotation;
	
	UPROPERTY(Replicated)
	float PositiveNegative;
	
	UPROPERTY(Replicated)
	float DoorCurrentRotation;

	UPROPERTY(Replicated)
	FVector DoorForwardVector;
};
