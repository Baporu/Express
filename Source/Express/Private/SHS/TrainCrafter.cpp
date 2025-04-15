// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCrafter.h"
#include "SHS/TrainCargo.h"
#include "SBS/Item.h"

void ATrainCrafter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ����� �� �ƴϸ� return
	if (!bIsMaking) return;

	MakeTimer += DeltaTime;

	if (MakeTimer >= MakeTime)
		MakeRail();
}

void ATrainCrafter::Init(ATrainEngine* EngineModule, float TrainSpeed, FVector Destination, ATrainCargo* CargoModule)
{
	TrainEngine = EngineModule;
	ModuleSpeed = TrainSpeed;
	NextPos = Destination;
	TrainCargo = CargoModule;
}

bool ATrainCrafter::CheckMakeRail()
{
	// ��ᰡ �����ϰų�
	if (TrainCargo->Woods.IsEmpty() || TrainCargo->Stones.IsEmpty())
		return false;

	// �̹� ������ �� ��������� return false
	if (Rails.Num() == MaxStackSize)
		return false;

	AItem* wood = TrainCargo->Woods.Top();
	TrainCargo->Woods.Pop();
	wood->Destroy();

	AItem* stone = TrainCargo->Stones.Top();
	TrainCargo->Stones.Pop();
	stone->Destroy();

	bIsMaking = true;

	return true;
}

AItem* ATrainCrafter::GetRail()
{
	if (Rails.IsEmpty()) return nullptr;

	AItem* rail = Rails.Top();
	rail->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	Rails.Pop();

	return rail;
}

void ATrainCrafter::MakeRail()
{
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += 10.0f * Rails.Num();

	AItem* rail = GetWorld()->SpawnActor<AItem>(BP_Rail, SpawnLocation, FRotator::ZeroRotator);
	rail->CreateItem(EItemType::Rail, 1);
	rail->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	Rails.Add(rail);

	bIsMaking = false;
	MakeTimer = 0.0f;

	// �� ���� �� ������ ��� ����
	CheckMakeRail();
}
