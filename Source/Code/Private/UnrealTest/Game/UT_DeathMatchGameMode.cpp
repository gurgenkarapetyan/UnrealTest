// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealTest/Game/UT_DeathMatchGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameStateBase.h"

#include "UnrealTest/Character/UT_PlayerState.h"
#include "UnrealTest/Character/UnrealTestCharacter.h"
#include "UnrealTest/Game/UT_DeathMatchGameState.h"
#include "UnrealTest/Components/UT_CustomPlayerStart.h"

AUT_DeathMatchGameMode::AUT_DeathMatchGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DefaultPawnClass = AUnrealTestCharacter::StaticClass();
	PlayerStateClass = AUT_PlayerState::StaticClass();
	GameStateClass = AUT_DeathMatchGameState::StaticClass();

	NumTeams = 2;
}

AActor* AUT_DeathMatchGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//Set Team
	AUT_PlayerState* NewPlayerState = Cast<AUT_PlayerState>(Player->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);
	
	//Choose start
	TArray<APlayerStart*> PossibleSpawns;
	APlayerStart* BestStart = nullptr;

	//Get All SpawnPoints from map
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* start = *It;
		if (start->IsA<AUT_CustomPlayerStart>())
		{
			//Check team restrictions
			if (CheckStartTeam(start, Player))
			{
				PossibleSpawns.Add(start);
			}
		}
	}

	//Choose Random Start Position
	if (!BestStart)
	{
		if (PossibleSpawns.Num() > 0)
		{
			BestStart = PossibleSpawns[FMath::RandHelper(PossibleSpawns.Num())];
		}
	}
	// In case no best start start by default
	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}

bool AUT_DeathMatchGameMode::CheckStartTeam(APlayerStart* PlayerStart, AController* Player) const
{
	if (Player)
	{
		AUT_CustomPlayerStart* customStart = Cast<AUT_CustomPlayerStart>(PlayerStart);
		AUT_PlayerState* playerState = Cast<AUT_PlayerState>(Player->PlayerState);

		if (playerState && customStart && customStart->GetSpawnTeam() == playerState->GetTeamNum())
		{
			return true;
		}
	}
	return false;
}

void AUT_DeathMatchGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (NumPlayers == 2)
	{
		OnMatchStart.Broadcast();
		StartMatch();
	}
}

int32 AUT_DeathMatchGameMode::ChooseTeam(AUT_PlayerState* PlayerState) const
{
	int32 PlayersTeam0 = 0;
	int32 PlayersTeam1 = 0;

	// Array of all Player States
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		if (AUT_PlayerState* State = Cast<AUT_PlayerState>(GameState->PlayerArray[i]))
		{
			// Add one in case 
			State->GetTeamNum() == 0 ? PlayersTeam0++ : PlayersTeam1++;
		}
	}
	
	// return the index of the team least populated
	return PlayersTeam0 <= PlayersTeam1 ? 0 : 1;
}

