// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainWaterTank.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainEngine.h"

// Called when the game starts or when spawned
void ATrainWaterTank::BeginPlay()
{
	Super::BeginPlay();

	TankMaterial = MeshComp->CreateDynamicMaterialInstance(0);
	//TrainEngine->FireTime += FireTime;
}

// Called every frame
void ATrainWaterTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Material 색상 변경하는 함수, 머티리얼에 파라미터 추가한 후 작동시켜야 함
	//ChangeTankColor();
}

void ATrainWaterTank::ChangeTankColor()
{
	float valueR = FireTimer / FireTime;
	float valueB = 1.0f - valueR;

	MaterialColor = FLinearColor(valueR, 0.0f, valueB);
	TankMaterial->SetVectorParameterValue(TEXT("TankColor"), MaterialColor);
}

