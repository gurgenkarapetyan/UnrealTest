// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "UT_DeathMatchGameMode.generated.h"

class AUT_PlayerState;
class APlayerStart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnd);
/**
 * 
 */
UCLASS()
class UNREALTEST_API AUT_DeathMatchGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUT_DeathMatchGameMode(const FObjectInitializer& ObjectInitializer);
	
protected:
	// PROPERTIES 
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 PlayerNumberToStartGame;
	
	//Number of teams
	int32 NumTeams;

	UPROPERTY(BlueprintAssignable)
	FOnMatchStart OnMatchStart;

	UPROPERTY(BlueprintAssignable)
	FOnMatchEnd OnMatchEnd;

	// Select best spawn point for player
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// Checks Player State team in spawning point 
	virtual bool CheckStartTeam(APlayerStart* PlayerStart, AController* Player) const;

	// New player joins
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	//TEAM FUNCTION
	//Picks team random or where there are the least Players
	int32 ChooseTeam(AUT_PlayerState* PlayerState) const;
};
