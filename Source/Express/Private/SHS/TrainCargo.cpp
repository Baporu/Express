// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCargo.h"
#include "Components/BoxComponent.h"
#include "SBS/Item.h"
#include "SHS/TrainEngine.h"

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

bool ATrainCargo::CheckAddResource()
{
	if (bOnFire) return false;
	return true;
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

void ATrainCargo::AddResource(AItem* Resource)
{
	switch (Resource->ItemType)
	{
		case EItemType::Wood:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Woods.Num() >= MaxCount) return;
			
			Woods.Add(Resource);
			Resource->MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Resource->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Resource->SetActorRelativeLocation(FVector(-40.0, 0.0, 0.0));
			Resource->SetActorLocation(GetActorLocation() + FVector(0.0, 0.0, 50.0) * Woods.Num());
			break;

		case EItemType::Stone:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Stones.Num() >= MaxCount) return;

			Stones.Add(Resource);
			Resource->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Resource->SetActorRelativeLocation(FVector(40.0, 0.0, 0.0));
			Resource->SetActorLocation(GetActorLocation() + FVector(0.0, 0.0, 50.0) * Stones.Num());
			break;

		// ��� �ڵ�
		default: break;
	}

	TrainEngine->CheckMakeRail();
}

AItem* ATrainCargo::GetResource(EItemType ResourceType)
{
	AItem* item = nullptr;

	// �ش� Ÿ���� �������� �����̳ʿ� ������ ���� ���� �����ۺ��� ��ȯ
	switch (ResourceType)
	{
		case EItemType::Wood:
			if (Woods.IsEmpty()) break;

			item = Woods.Top();
			item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Woods.Pop();
			break;

		case EItemType::Stone:
			if (Stones.IsEmpty()) break;

			item = Stones.Top();
			item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Stones.Pop();
			break;
		
		// ��� �ڵ�, ������ �� �ƴϸ� �� ������ ��ȯ
		default: break;
	}

	// �ش��ϴ� Ÿ���� ������(������ nullptr)�� ��ȯ
	return item;
}
