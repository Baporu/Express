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
			// 컨테이너가 이미 최대 수량이면 못 받고 return
			if (Woods.Num() >= MaxCount) return;
			
			Woods.Add(Resource);
			Resource->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			Resource->SetActorRelativeLocation(Resource->GetActorLocation() + FVector(0.0, 0.0, 20.0) * Woods.Num());
			break;

		case EItemType::Stone:
			// 컨테이너가 이미 최대 수량이면 못 받고 return
			if (Stones.Num() >= MaxCount) return;

			Stones.Add(Resource);
			Resource->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			Resource->SetActorRelativeLocation(Resource->GetActorLocation() + FVector(0.0, 0.0, 20.0) * Stones.Num());
			break;

		// 방어 코드
		default: break;
	}
}

AItem* ATrainCargo::GetResource(EItemType ResourceType)
{
	AItem* item = nullptr;

	// 해당 타입의 아이템이 컨테이너에 있으면 가장 끝의 아이템부터 반환
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
		
		// 방어 코드, 나무나 돌 아니면 널 포인터 반환
		default: break;
	}

	// 해당하는 타입의 아이템(없으면 nullptr)을 반환
	return item;
}
