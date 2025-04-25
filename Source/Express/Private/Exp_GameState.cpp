// Fill out your copyright notice in the Description page of Project Settings.


#include "Exp_GameState.h"
#include "../Express.h"
#include "SHS/TrainEngine.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerController.h"

void AExp_GameState::SetTrainEngine(class ATrainEngine* Engine) {
	if (!TrainEngine) TrainEngine = Engine;
}

void AExp_GameState::AccelTrain() {
	if (!HasAuthority())
		UE_LOG(LogTrain, Error, TEXT("CLIENT TRIED TO ACCESS THIS FUNCTION"));

	DisablePlayersInput();
	TrainEngine->AccelModules();
}

void AExp_GameState::DecelTrain() {
	if (!HasAuthority())
		UE_LOG(LogTrain, Error, TEXT("CLIENT TRIED TO ACCESS THIS FUNCTION"));

	TrainEngine->DecelModules();
}

void AExp_GameState::DisablePlayersInput() {
	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (pc) { pc->Client_DisableInput(); PRINTLOG(TEXT("Trying to disable players' input..")); }
	}
}
