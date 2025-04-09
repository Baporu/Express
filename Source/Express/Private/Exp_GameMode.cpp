// Fill out your copyright notice in the Description page of Project Settings.


#include "Exp_GameMode.h"
#include "SHS/TrainWaterTank.h"

void AExp_GameMode::CoolDownTank()
{
	if (!WaterTank) return;

	WaterTank->EndFire();
}
