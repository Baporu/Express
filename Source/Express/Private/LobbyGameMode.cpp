// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = false;
}
void ALobbyGameMode::GoToMap()
{
	GetWorld()->ServerTravel(TEXT("/Game/SBS/SBS_Level?listen"));
}
