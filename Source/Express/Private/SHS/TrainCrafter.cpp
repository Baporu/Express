// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCrafter.h"
#include "SHS/TrainCargo.h"

void ATrainCrafter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 만드는 중 아니면 return
	if (!bIsMaking) return;


	if (TrainCargo->Woods.IsEmpty() || TrainCargo->Stones.IsEmpty())
		return;

	if (Rails.Num() == MaxStackSize)
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
	// 만드는 중에 또 만들면 안 되니까 true로 설정
	bIsMaking = true;

	TrainCargo->Woods.Pop();
	TrainCargo->Stones.Pop();

	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += 10.0f * Rails.Num();

	AActor* rail = GetWorld()->SpawnActor<AActor>(BP_Rail, SpawnLocation, FRotator::ZeroRotator);
	rail->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	Rails.Add(rail);

	MakeTimer = 0.0f;
}
