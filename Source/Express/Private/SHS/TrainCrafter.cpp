// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCrafter.h"
#include "SHS/TrainCargo.h"
#include "SBS/Item.h"
#include "../Express.h"

void ATrainCrafter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ����� �� �ƴϸ� return
	if (!bIsMaking) return;

	if (bOnFire) return;

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
	UE_LOG(LogTrain, Warning, TEXT("Wood Destroyed"));

	AItem* stone = TrainCargo->Stones.Top();
	TrainCargo->Stones.Pop();
	stone->Destroy();
	UE_LOG(LogTrain, Warning, TEXT("Stone Destroyed"));

	bIsMaking = true;

	return true;
}

bool ATrainCrafter::CheckRail()
{
	if (!Rails.IsEmpty()) return true;

	return false;
}

TArray<AItem*> ATrainCrafter::GetRail()
{
	Rails[0]->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	return Rails;
}

void ATrainCrafter::MakeRail()
{
	AItem* rail = GetWorld()->SpawnActor<AItem>(BP_Rail, GetActorLocation(), GetActorRotation());
	rail->CreateItem(EItemType::Rail);

	if (!CheckRail()) {
		rail->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
		rail->SetActorRelativeLocation(FVector(0.0, 0.0, 50.0));
	}
	else
		rail->AttachToActor(Rails.Top(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("ItemHead")));
	Rails.Add(rail);


	bIsMaking = false;
	MakeTimer = 0.0f;

	// �� ���� �� ������ ��� ����
	CheckMakeRail();
}
