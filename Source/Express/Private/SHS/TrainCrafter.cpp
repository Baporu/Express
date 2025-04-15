// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCrafter.h"
#include "SHS/TrainCargo.h"

void ATrainCrafter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TrainCargo->Woods.IsEmpty() || TrainCargo->Irons.IsEmpty())
		return;

	if (StackSize == MaxStackSize)
		return;

	MakeTimer += DeltaTime;

	if (MakeTimer >= DeltaTime)
		MakeRail();
}

void ATrainCrafter::Init(ATrainEngine* EngineModule, float TrainSpeed, FVector Destination, ATrainCargo* CargoModule)
{
	TrainEngine = EngineModule;
	ModuleSpeed = TrainSpeed;
	NextPos = Destination;
	TrainCargo = CargoModule;
}

void ATrainCrafter::MakeRail()
{
	TrainCargo->Woods.Pop();
	TrainCargo->Irons.Pop();

	StackSize++;

	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += 10.0f * StackSize;

	GetWorld()->SpawnActor<AActor>(BP_Rail, SpawnLocation, FRotator::ZeroRotator);

	MakeTimer = 0.0f;
}
