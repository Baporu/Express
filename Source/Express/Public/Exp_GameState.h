// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Exp_GameState.generated.h"

/**
 * 
 */
UCLASS()
class EXPRESS_API AExp_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_GameCleared)
	bool bIsGameCleared = false;
	UPROPERTY(ReplicatedUsing=OnRep_GameFailed)
	bool bIsGameFailed = false;

	void SetTrainEngine(class ATrainEngine* Engine);
	void AccelTrain();
	void DecelTrain();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnServer_GameEnd(bool bIsCleared);
	void ResetInputMode();

private:
	class ATrainEngine* TrainEngine;

	void DisablePlayersInput();

	UFUNCTION()
	void OnRep_GameCleared();
	UFUNCTION()
	void OnRep_GameFailed();
};
