// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealTest/Components/UT_CustomPlayerStart.h"

AUT_CustomPlayerStart::AUT_CustomPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("AUT_CustomPlayerStart::AUT_CustomPlayerStart"));
	SpawnTeam = -1;
}
