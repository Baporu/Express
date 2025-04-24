// Fill out your copyright notice in the Description page of Project Settings.


#include "Exp_GameState.h"
#include "../Express.h"
#include "SHS/TrainEngine.h"

void AExp_GameState::SetTrainEngine(class ATrainEngine* Engine) {
	if (!TrainEngine) TrainEngine = Engine;
}

void AExp_GameState::AccelTrain() {
	if (!HasAuthority())
		UE_LOG(LogTrain, Error, TEXT("CLIENT TRIED TO ACCESS THIS FUNCTION"));

	TrainEngine->AccelModules();
}

void AExp_GameState::DecelTrain() {
	if (!HasAuthority())
		UE_LOG(LogTrain, Error, TEXT("CLIENT TRIED TO ACCESS THIS FUNCTION"));

	TrainEngine->DecelModules();
}
