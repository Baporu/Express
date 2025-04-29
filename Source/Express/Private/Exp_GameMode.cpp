// Fill out your copyright notice in the Description page of Project Settings.


#include "Exp_GameMode.h"
#include "SHS/TrainWaterTank.h"
#include "NetPlayerController.h"
#include "Exp_GameState.h"

void AExp_GameMode::CoolDownTank()
{
	if (!WaterTank) return;

	WaterTank->EndFire();
}

void AExp_GameMode::AccelTrain() {
	Cast<AExp_GameState>(GameState)->AccelTrain();
}

void AExp_GameMode::DecelTrain() {
	Cast<AExp_GameState>(GameState)->DecelTrain();
}

void AExp_GameMode::GoToMap()
{
	GetWorld()->ServerTravel(TEXT("/Game/SBS/SBS_Level?listen"));
}

void AExp_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ANetPlayerController* pc = Cast<ANetPlayerController>(NewPlayer))
	{
		pc->SetMainCamera();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PostLogin: Invalid PlayerController for %s"), *NewPlayer->GetName());
	}
}
