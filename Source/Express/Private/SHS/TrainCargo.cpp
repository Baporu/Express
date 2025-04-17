// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCargo.h"
#include "Components/BoxComponent.h"
#include "SBS/Item.h"
#include "SHS/TrainEngine.h"
#include "../Express.h"

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
	// 나무나 돌이면 추가할 수 있는 자원
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
	switch (Resources[0]->ItemType)
	{
		case EItemType::Wood:
			// 컨테이너가 이미 최대 수량이면 못 받고 return
			if (Woods.Num() > MaxCount) return;
			
			UE_LOG(LogTrain, Warning, TEXT("Wood Added"));

			Woods.Append(Resources);
			Resources[0]->SetActorRotation(FRotator::ZeroRotator);
			Resources[0]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Resources[0]->SetActorLocation(GetActorLocation() + FVector(-40.0, 0.0, 50.0));

			break;

		case EItemType::Stone:
			// 컨테이너가 이미 최대 수량이면 못 받고 return
			if (Stones.Num() > MaxCount) return;

			Stones.Append(Resources);
			Resources[0]->SetActorRotation(FRotator::ZeroRotator);
			Resources[0]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Resources[0]->SetActorLocation(GetActorLocation() + FVector(40.0, 0.0, 50.0));
			break;

		// 방어 코드
		default: return;
	}

	TrainEngine->CheckMakeRail();
}

TArray<AItem*> ATrainCargo::GetResource(EItemType ResourceType)
{
	TArray<AItem*> items;

	// 해당 타입의 아이템이 컨테이너에 있으면 가장 끝의 아이템부터 반환
	switch (ResourceType)
	{
		case EItemType::Wood:
			if (Woods.IsEmpty()) break;

			items = Woods;
			items[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Woods.Empty();
			break;

		case EItemType::Stone:
			if (Stones.IsEmpty()) break;

			items = Stones;
			items[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Stones.Empty();
			break;
		
		// 방어 코드, 나무나 돌 아니면 널 포인터 반환
		default: break;
	}

	// 해당하는 타입의 아이템(없으면 nullptr)을 반환
	return items;
}
