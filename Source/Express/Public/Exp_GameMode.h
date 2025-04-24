// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Exp_GameMode.generated.h"

/**
 * 
 */


UCLASS()
class EXPRESS_API AExp_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	class ATrainWaterTank* WaterTank;
	bool bIsToolsSpawned = false;
	UFUNCTION(Exec)
	void CoolDownTank();
	UFUNCTION(Exec)
	void AccelTrain();
	UFUNCTION(Exec)
	void DecelTrain();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};

