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

	// 처음에 타일 정보 받아와서 계산
	NextPos = GetActorLocation();
	NextRot = GetActorRotation().Yaw;

	Init();

	SpawnDefaultModules();
}

// Called every frame
void ATrainEngine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FVector::Dist2D(GetActorLocation(), NextPos) <= 0.5)
		GetTileLocation();
}

void ATrainEngine::Init()
{
	// 다음 위치를 타일한테서 받아옴
	if (FMath::RandBool()) {
		NextPos.X += 100.0f;
		NextRot = 0.0;
	}
	else {
		NextPos.Y += 100.0f;
		NextRot = 90.0;
	}

	UE_LOG(LogTrain, Log, TEXT("Next Position Changed"));
}

bool ATrainEngine::CheckModule(int32 ModuleIndex)
{
	if (TrainModules.Num() == ModuleIndex)
		return false;

	return true;
}

ATrainModule* ATrainEngine::GetFrontModule(int32 CurrentModuleIndex)
{
	if (CurrentModuleIndex < 1)
		return nullptr;

	return TrainModules[CurrentModuleIndex - 1];
}

ATrainModule* ATrainEngine::GetBackModule(int32 CurrentModuleIndex)
{
	if (TrainModules.Num() == CurrentModuleIndex + 1)
		return nullptr;

	return TrainModules[CurrentModuleIndex + 1];
}

void ATrainEngine::AttachModule(ATrainModule* TrainModule)
{
//	TrainModule->AttachToComponent(ModuleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TrainModule->SetActorLocation(ModuleComp->GetComponentLocation());
	TrainModule->SetActorRotation(GetActorRotation());
	TrainModules.Add(TrainModule);
}

void ATrainEngine::AttachModule(ATrainModule* TrainModule, int32 AttachIndex)
{
	USceneComponent* AttachComp = TrainModules[AttachIndex]->GetModuleComp();

//	TrainModule->AttachToComponent(AttachComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TrainModule->SetActorLocation(AttachComp->GetComponentLocation());
	TrainModule->SetActorRotation(GetActorRotation());
	TrainModules.Add(TrainModule);
}

void ATrainEngine::AddFireTime(float WaterTankTime)
{
	FireTime += WaterTankTime;
	UE_LOG(LogTrain, Log, TEXT("Fire Time Added"));
}

void ATrainEngine::MoveTrain(float DeltaTime)
{
	FVector dir = NextPos - GetActorLocation();
	FVector vt = dir.GetSafeNormal() * TrainSpeed * DeltaTime;
	SetActorLocation(GetActorLocation() + vt);
}

// void ATrainEngine::RotateTrain(float DeltaTime)
// {
// 	double curRot = GetActorRotation().Yaw;
// 	curRot = FMath::Lerp(curRot, NextRot, DeltaTime / 4);
// 
// 	SetActorRotation(FRotator(0.0, curRot, 0.0));
// }

void ATrainEngine::OnFire(float DeltaTime)
{
	TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;
}

// ================================ 임시 함수들 ================================
void ATrainEngine::GetTileLocation()
{	
	// 현재 위치를 다음 모듈에게 넘겨주고
	TrainModules[1]->SetModuleLocation(NextPos);
	TrainModules[1]->SetModuleRotation(NextRot);

	// 다음 위치를 타일한테서 받아옴
	if (FMath::RandBool()) {
		NextPos.X += 100.0f;
		NextRot = 0.0;
	}
	else {
		NextPos.Y += 100.0f;
		NextRot = 90.0;
	}

	UE_LOG(LogTrain, Log, TEXT("Next Position Changed"));
}

void ATrainEngine::SpawnDefaultModules()
{
	ATrainWaterTank* WaterTank = GetWorld()->SpawnActorDeferred<ATrainWaterTank>(BP_WaterTank, GetActorTransform());
	WaterTank->Init(this, TrainSpeed, GetActorLocation());
	WaterTank->FinishSpawning(GetActorTransform());

	ATrainCargo* Cargo = GetWorld()->SpawnActorDeferred<ATrainCargo>(BP_Cargo, GetActorTransform());
	Cargo->Init(this, TrainSpeed, GetActorLocation());
	Cargo->FinishSpawning(GetActorTransform());

	ATrainCrafter* Crafter = GetWorld()->SpawnActorDeferred<ATrainCrafter>(BP_Crafter, GetActorTransform());
	Crafter->Init(this, TrainSpeed, GetActorLocation(), Cargo);
	Crafter->FinishSpawning(GetActorTransform());

	AttachModule(WaterTank);
	WaterTank->SetModuleIndex(1);
	AttachModule(Cargo, 1);
	Cargo->SetModuleIndex(2);
	AttachModule(Crafter, 2);
	Crafter->SetModuleIndex(3);
}

