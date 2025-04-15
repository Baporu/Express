// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCargo.h"
#include "Components/BoxComponent.h"
#include "SBS/Item.h"

ATrainCargo::ATrainCargo()
{
	WoodComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WoodComp"));
	WoodComp->SetupAttachment(MeshComp);
	WoodComp->SetBoxExtent(FVector(50.0, 25.0, 50.0));
	WoodComp->SetRelativeLocation(FVector(0.0, -25.0, 0.0));

	StoneComp = CreateDefaultSubobject<UBoxComponent>(TEXT("IronComp"));
	StoneComp->SetupAttachment(MeshComp);
	StoneComp->SetBoxExtent(FVector(50.0, 25.0, 50.0));
	StoneComp->SetRelativeLocation(FVector(0.0, 25.0, 0.0));
}

void ATrainCargo::AddResource(AItem* Resource)
{
	switch (Resource->ItemType)
	{
		case EItemType::Wood:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Woods.Num() >= MaxCount) return;
			
			Woods.Add(Resource);
			Resource->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			Resource->SetActorRelativeLocation(Resource->GetActorLocation() + FVector(0.0, 0.0, 20.0) * Woods.Num());
			break;

		case EItemType::Stone:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Stones.Num() >= MaxCount) return;

			Stones.Add(Resource);
			Resource->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			Resource->SetActorRelativeLocation(Resource->GetActorLocation() + FVector(0.0, 0.0, 20.0) * Stones.Num());
			break;

		// ��� �ڵ�
		default: break;
	}
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
			item->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			Woods.Pop();
			break;

		case EItemType::Stone:
			if (Stones.IsEmpty()) break;

			item = Stones.Top();
			item->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			Stones.Pop();
			break;
		
		// ��� �ڵ�, ������ �� �ƴϸ� �� ������ ��ȯ
		default: break;
	}

	// �ش��ϴ� Ÿ���� ������(������ nullptr)�� ��ȯ
	return item;
}
