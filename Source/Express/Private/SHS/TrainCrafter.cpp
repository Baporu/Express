// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCrafter.h"
#include "SHS/TrainCargo.h"

void ATrainCrafter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ����� �� �ƴϸ� return
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
	// ����� �߿� �� ����� �� �Ǵϱ� true�� ����
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
