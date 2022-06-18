// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UT_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTEST_API AUT_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AUT_PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Updates Mesh Colors With the team id -- Red or Blue
	void UpdateTeamColors() const;
	
	// Sets Team
	void SetTeamNum(int32 NewTeamNumber);
	
	// Get Team
	UFUNCTION(BlueprintCallable)
	int32 GetTeamNum() const;
	
	UFUNCTION()
	void OnRep_TeamNumberChanged();
	
	// Team ID
	UPROPERTY(ReplicatedUsing = OnRep_TeamNumberChanged)
	int32 TeamNumber;
	
	// Times has died this pawn
	UPROPERTY(Replicated)
	int32 NumDeaths;
};
