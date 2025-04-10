// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainEngine.h"
#include "Express/Express.h"

// Sets default values
ATrainEngine::ATrainEngine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrainModules.Add(this);
	ModuleNumber = 0;
}

// Called when the game starts or when spawned
void ATrainEngine::BeginPlay()
{
	Super::BeginPlay();

	// 처음에 타일 정보 받아와서 계산
	NextPos = GetActorLocation();
	GetTileLocation();
}

// Called every frame
void ATrainEngine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector dir = NextPos - GetActorLocation();
	FVector vt = dir.GetSafeNormal2D() * TrainSpeed * DeltaTime;
	SetActorLocation(GetActorLocation() + vt);

	if (FVector::Dist2D(GetActorLocation(), NextPos) <= 0.5)
		GetTileLocation();

	RotateTrain(DeltaTime);
}

bool ATrainEngine::CheckModule(int32 ModuleIndex)
{
	if (TrainModules.Num() == ModuleIndex)
		return false;

	return true;
}

ATrainModule* ATrainEngine::GetFrontModule(int32 ModuleIndex)
{
	if (ModuleIndex < 1)
		return nullptr;

	return TrainModules[ModuleIndex - 1];
}

ATrainModule* ATrainEngine::GetBackModule(int32 ModuleIndex)
{
	if (TrainModules.Num() == ModuleIndex + 1)
		return nullptr;

	return TrainModules[ModuleIndex + 1];
}

void ATrainEngine::AddFireTime(float WaterTankTime)
{
	FireTime += WaterTankTime;
	UE_LOG(LogTrain, Log, TEXT("Fire Time Added"));
}

void ATrainEngine::OnFire(float DeltaTime)
{
	TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;
}

// ================================ 임시 함수들 ================================
void ATrainEngine::GetTileLocation()
{	
	if (FMath::RandBool()) {
		NextPos.X += 100.0f;
		NextRot = 0.0;
		rotA = 0.0;
	}
	else {
		NextPos.Y += 100.0f;
		NextRot = 90.0;
		rotA = 0.0;
	}

	UE_LOG(LogTrain, Log, TEXT("Next Position Changed"));
}

void ATrainEngine::RotateTrain(float DeltaTime)
{
	rotA += DeltaTime;
	double curRot = GetActorRotation().Yaw;

	curRot = FMath::Lerp(curRot, NextRot, rotA / 4);

	SetActorRotation(FRotator(0.0, curRot, 0.0));
}

