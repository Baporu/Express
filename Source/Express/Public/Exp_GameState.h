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
	void SetTrainEngine(class ATrainEngine* Engine);
	void AccelTrain();
	void DecelTrain();

private:
	class ATrainEngine* TrainEngine;
};
