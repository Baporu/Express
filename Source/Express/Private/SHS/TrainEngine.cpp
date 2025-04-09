// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainEngine.h"
#include "Express/Express.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainWaterTank.h"
#include "SHS/TrainCargo.h"
#include "SHS/TrainCrafter.h"

// Sets default values
ATrainEngine::ATrainEngine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrainEngine = this;
	TrainModules.Add(this);
	ModuleNumber = 0;
}

// Called when the game starts or when spawned
void ATrainEngine::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultModules();

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

void ATrainEngine::AttachModule(ATrainModule* TrainModule)
{
	TrainModule->AttachToComponent(ModuleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TrainModules.Add(TrainModule);
}

void ATrainEngine::AttachModule(ATrainModule* TrainModule, int32 AttachIndex)
{
	USceneComponent* AttachComp = TrainModules[AttachIndex]->GetModuleComp();

	TrainModule->AttachToComponent(AttachComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TrainModules.Add(TrainModule);
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

void ATrainEngine::SpawnDefaultModules()
{
	ATrainWaterTank* WaterTank = GetWorld()->SpawnActorDeferred<ATrainWaterTank>(BP_WaterTank, GetActorTransform());
	WaterTank->Init(this);
	WaterTank->FinishSpawning(GetActorTransform());

	ATrainCargo* Cargo = GetWorld()->SpawnActorDeferred<ATrainCargo>(BP_Cargo, GetActorTransform());
	Cargo->Init(this);
	Cargo->FinishSpawning(GetActorTransform());

	ATrainCrafter* Crafter = GetWorld()->SpawnActorDeferred<ATrainCrafter>(BP_Crafter, GetActorTransform());
	Crafter->Init(this, Cargo);
	Crafter->FinishSpawning(GetActorTransform());

	AttachModule(WaterTank);
	WaterTank->SetModuleIndex(1);
	AttachModule(Cargo, 1);
	Cargo->SetModuleIndex(2);
	AttachModule(Crafter, 2);
	Crafter->SetModuleIndex(3);
}

