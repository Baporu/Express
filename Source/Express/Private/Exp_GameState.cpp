// Fill out your copyright notice in the Description page of Project Settings.


#include "Exp_GameState.h"
#include "../Express.h"
#include "SHS/TrainEngine.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerController.h"
#include "Net/UnrealNetwork.h"

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

void AExp_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExp_GameState, bIsGameCleared);
	DOREPLIFETIME(AExp_GameState, bIsGameFailed);
}

void AExp_GameState::OnServer_GameEnd(bool bIsCleared) {
	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (!pc || !pc->IsLocalController()) continue;

		bIsCleared ? pc->Client_ShowClearUI() : pc->Client_ShowFailUI();

		return;
	}
}

void AExp_GameState::DisablePlayersInput() {
	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (pc) { pc->Client_DisableInput(); PRINTLOG(TEXT("Trying to disable players' input..")); }
	}
}

void AExp_GameState::ResetInputMode() {
	PRINTLOG(TEXT("Input Mode Reset Started"));

	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (pc) { pc->Client_ResetInputMode(); }
	}

	PRINTLOG(TEXT("Input Mode Reset Finished"));
}

void AExp_GameState::Server_CheckLoading_Implementation() {
	bool bFullyLoaded = true;

	PRINTTRAIN(TEXT("Check Loading Progress"));
	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (!pc) continue;

		bFullyLoaded &= pc->bIsLoaded;

		// false 됐으면 더 볼 필요 없으니까 return
		if (!bFullyLoaded) return;
	}

	// 전부 로드됐으면 기차 출발 (false는 for문 안에서 return됨)
	if (TrainEngine->GetIsStarted()) return;
	TrainEngine->SetInitTimer();
	PRINTTRAIN(TEXT("Set Train Timer"));
}

void AExp_GameState::OnRep_GameCleared() {
	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (pc) { pc->Client_ShowClearUI(); PRINTLOG(TEXT("Trying to show clear UI..")); }
	}
}

void AExp_GameState::OnRep_GameFailed() {
	for (APlayerState* ps : PlayerArray) {
		auto pc = Cast<ANetPlayerController>(ps->GetOwner());

		if (pc) { pc->Client_ShowFailUI(); PRINTLOG(TEXT("Trying to show fail ui..")); }
	}
}
