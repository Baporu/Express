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
	switch (Resources[0]->ItemType)
	{
		case EItemType::Wood:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Woods.Num() > MaxCount) return;
			
			UE_LOG(LogTrain, Warning, TEXT("Wood Added"));

			Woods.Append(Resources);
			Resources[0]->SetActorRotation(FRotator::ZeroRotator);
			Resources[0]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Resources[0]->SetActorLocation(GetActorLocation() + FVector(-40.0, 0.0, 50.0));

			break;

		case EItemType::Stone:
			// �����̳ʰ� �̹� �ִ� �����̸� �� �ް� return
			if (Stones.Num() > MaxCount) return;

			Stones.Append(Resources);
			Resources[0]->SetActorRotation(FRotator::ZeroRotator);
			Resources[0]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Resources[0]->SetActorLocation(GetActorLocation() + FVector(40.0, 0.0, 50.0));
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
			items[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Woods.Empty();
			break;

		case EItemType::Stone:
			if (Stones.IsEmpty()) break;

			items = Stones;
			items[0]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			Stones.Empty();
			break;
		
		// ��� �ڵ�, ������ �� �ƴϸ� �� ������ ��ȯ
		default: break;
	}

	// �ش��ϴ� Ÿ���� ������(������ nullptr)�� ��ȯ
	return items;
}
