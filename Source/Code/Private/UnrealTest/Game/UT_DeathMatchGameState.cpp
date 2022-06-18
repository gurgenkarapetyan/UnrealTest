// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealTest/Game/UT_DeathMatchGameState.h"
#include "Net/UnrealNetwork.h"


AUT_DeathMatchGameState::AUT_DeathMatchGameState()
{
	UE_LOG(LogTemp, Warning, TEXT("AUT_DeathMatchGameState::AUT_DeathMatchGameState"));

	NumTeams = 2;
	TeamScores.AddZeroed(NumTeams);
}

void AUT_DeathMatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UE_LOG(LogTemp, Warning, TEXT("AUT_DeathMatchGameState::GetLifetimeReplicatedProps"));

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUT_DeathMatchGameState, NumTeams);
	DOREPLIFETIME(AUT_DeathMatchGameState, TeamScores);
}

TArray<int32> AUT_DeathMatchGameState::GetTeamScores() const
{
	UE_LOG(LogTemp, Warning, TEXT("AUT_DeathMatchGameState::GetTeamScores"));

	return TeamScores;
}
