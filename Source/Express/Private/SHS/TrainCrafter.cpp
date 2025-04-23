// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCrafter.h"
#include "SHS/TrainCargo.h"
#include "SBS/Item.h"
#include "Express/Express.h"

void ATrainCrafter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �������� ����
	if (!HasAuthority()) return;

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

void ATrainCrafter::CheckMakeRail()
{
	Server_CheckMakeRail();
}

bool ATrainCrafter::CheckRail()
{
	if (!Rails.IsEmpty()) return true;

	return false;
}

TArray<AItem*> ATrainCrafter::GetRail()
{
	TArray<AItem*> items;

	items = Rails;
	Multicast_DetachRail(items[0]);
	Rails.Empty();

	return items;
}

void ATrainCrafter::MakeRail()
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	AItem* rail = GetWorld()->SpawnActor<AItem>(BP_Rail, GetActorLocation(), GetActorRotation());
	rail->CreateItem(EItemType::Rail);

	if (!CheckRail()) {
		Multicast_AttachRail(rail, this);
		rail->SetActorRelativeLocation(FVector(0.0, 0.0, 50.0));
	}
	else
		Multicast_AttachRail(rail, Rails.Top());
	Rails.Add(rail);


	bIsMaking = false;
	MakeTimer = 0.0f;

	// �� ���� �� ������ ��� ����
	CheckMakeRail();
}

void ATrainCrafter::Server_CheckMakeRail_Implementation() {
	// ��ᰡ �����ϰų�
	if (TrainCargo->Woods.IsEmpty() || TrainCargo->Stones.IsEmpty())
		return;

	// �̹� ����� �־ return
	if (bIsMaking) return;

	// �̹� ������ �� ��������� return false
	if (Rails.Num() == MaxStackSize)
		return;

	AItem* wood = TrainCargo->Woods.Top();
	TrainCargo->Woods.Pop();
	wood->Destroy();
	UE_LOG(LogTrain, Warning, TEXT("Wood Destroyed"));

	AItem* stone = TrainCargo->Stones.Top();
	TrainCargo->Stones.Pop();
	stone->Destroy();
	UE_LOG(LogTrain, Warning, TEXT("Stone Destroyed"));

	bIsMaking = true;

	return;
}

void ATrainCrafter::Multicast_AttachRail_Implementation(AItem* Rail, AActor* ParentActor) {
	Rail->AttachToActor(ParentActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("ItemHead")));
}

void ATrainCrafter::Multicast_DetachRail_Implementation(AItem* Rail) {
	Rail->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}
