// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealTest/Character/UT_PlayerState.h"

#include "Net/UnrealNetwork.h"	

#include "UnrealTest/Character/UnrealTestCharacter.h"

AUT_PlayerState::AUT_PlayerState()
{
	TeamNumber = 0;
}

void AUT_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUT_PlayerState, TeamNumber);
}

void AUT_PlayerState::OnRep_TeamNumberChanged()
{
	// In case team has changed update colours
	UpdateTeamColors();
}

void AUT_PlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
	UpdateTeamColors();
}

int32 AUT_PlayerState::GetTeamNum() const
{
	return TeamNumber;
}

void AUT_PlayerState::UpdateTeamColors() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (AUnrealTestCharacter* playerChar = Cast<AUnrealTestCharacter>(OwnerController->GetCharacter()))
		{
			playerChar->UpdateTeamColors(TeamNumber);
		}
	}
}
