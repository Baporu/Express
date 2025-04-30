// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCargo.h"
#include "Components/BoxComponent.h"
#include "SBS/Item.h"
#include "SHS/TrainEngine.h"
#include "Express/Express.h"

ATrainCargo::ATrainCargo()
{
	WoodComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WoodComp"));
	WoodComp->SetupAttachment(RootComponent);
	WoodComp->SetBoxExtent(FVector(50.0, 25.0, 50.0));
	WoodComp->SetRelativeLocation(FVector(0.0, -25.0, 0.0));

	StoneComp = CreateDefaultSubobject<UBoxComponent>(TEXT("IronComp"));
	StoneComp->SetupAttachment(RootComponent);
	StoneComp->SetBoxExtent(FVector(50.0, 25.0, 50.0));
	StoneComp->SetRelativeLocation(FVector(0.0, 25.0, 0.0));
}

bool ATrainCargo::CheckInteraction()
{
	return bOnFire ? false : true;
}

bool ATrainCargo::CheckAddResource(EItemType ResourceType)
{
	// ������ ���̸� �߰��� �� �ִ� �ڿ�
	switch (ResourceType)
	{
		case EItemType::Wood:	return true;
		case EItemType::Stone:	return true;

		default: return false;
	}
}

bool ATrainCargo::CheckGetResource(EItemType ResourceType)
{
	switch (ResourceType)
	{
		case EItemType::Wood:
			if (!Woods.IsEmpty()) return true;
			return false;

		case EItemType::Stone:
			if (!Stones.IsEmpty()) return true;
			return false;

		default:
			return false;
	}
}

void ATrainCargo::AddResource(TArray<AItem*> Resources)
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	switch (Resources[0]->ItemType)
	{
		case EItemType::Wood:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Woods.Num() > MaxCount) return;

			Resources[0]->SetActorRotation(FRotator::ZeroRotator);

			if (Woods.IsEmpty()) {
				Client_AttachResource(Resources[0]);
				Resources[0]->SetActorLocation(GetActorLocation() + FVector(-40.0, 0.0, 70.0));
			}
			else {
				Client_AttachResourceToParent(Resources[0], Woods.Top());
			}

			Woods.Append(Resources);
			break;

		case EItemType::Stone:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Stones.Num() > MaxCount) return;

			Resources[0]->SetActorRotation(FRotator::ZeroRotator);
			
			if (Stones.IsEmpty()) {
				Client_AttachResource(Resources[0]);
				Resources[0]->SetActorLocation(GetActorLocation() + FVector(40.0, 0.0, 70.0));
			}
			else {
				Client_AttachResourceToParent(Resources[0], Stones.Top());
			}

			Stones.Append(Resources);
			break;

		// ��� �ڵ�
		default: return;
	}

	TrainEngine->CheckMakeRail();
}

TArray<AItem*> ATrainCargo::GetResource(EItemType ResourceType)
{
	TArray<AItem*> items;

	// �ش� Ÿ���� �������� �����̳ʿ� ������ ���� ���� �����ۺ��� ��ȯ
	switch (ResourceType)
	{
		case EItemType::Wood:
			if (Woods.IsEmpty()) break;

			items = Woods;
			Client_DetachResource(items[0]);
			Woods.Empty();
			break;

		case EItemType::Stone:
			if (Stones.IsEmpty()) break;

			items = Stones;
			Client_DetachResource(items[0]);
			Stones.Empty();
			break;
		
		// ��� �ڵ�, ������ �� �ƴϸ� �� ������ ��ȯ
		default: break;
	}

	// �ش��ϴ� Ÿ���� ������(������ nullptr)�� ��ȯ
	return items;
}

void ATrainCargo::Client_AttachResource_Implementation(AItem* Resource) {
	Resource->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ATrainCargo::Client_AttachResourceToParent_Implementation(AItem* Resource, AActor* ParentActor) {
	Resource->AttachToActor(ParentActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("ItemHead"));
}

void ATrainCargo::Client_DetachResource_Implementation(AItem* Resource) {
	Resource->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}
