// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "UT_DeathMatchGameState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTEST_API AUT_DeathMatchGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AUT_DeathMatchGameState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	// Number of teams in current game
	UPROPERTY(Replicated)
	int32 NumTeams;
};
