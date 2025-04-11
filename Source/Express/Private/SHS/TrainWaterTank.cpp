// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainWaterTank.h"
#include "Express/Express.h"
#include "SHS/TrainEngine.h"
#include "Exp_GameMode.h"

// Called when the game starts or when spawned
void ATrainWaterTank::BeginPlay()
{
	Super::BeginPlay();

	TankMaterial = MeshComp->CreateDynamicMaterialInstance(0);

	AExp_GameMode* gm = Cast<AExp_GameMode>(GetWorld()->GetAuthGameMode());
	if (gm) gm->WaterTank = this;

	if (!TrainEngine) {
		UE_LOG(LogTrain, Log, TEXT("Train Engine Not Found"));

		return;
	}

	UE_LOG(LogTrain, Log, TEXT("Try Attach WaterTank To Engine"));
	TrainEngine->AttachModule(this);

	UE_LOG(LogTrain, Log, TEXT("Try Add Fire Time To Engine"));
	TrainEngine->AddFireTime(FireTime);
}

// Called every frame
void ATrainWaterTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTimer += DeltaTime;

	ChangeTankColor();
}

void ATrainWaterTank::EndFire()
{
	if (bOnFire) {
		Super::EndFire();

		return;
	}

	FireTimer = 0.0f;
}

void ATrainWaterTank::ChangeTankColor()
{
	float valueR = FireTimer / FireTime;
	float valueB = 1.0f - valueR;

	MaterialColor = FLinearColor(valueR, 0.0f, valueB);
	TankMaterial->SetVectorParameterValue(TEXT("TankColor"), MaterialColor);
}

