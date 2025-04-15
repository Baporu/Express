// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainCargo.h"
#include "Components/BoxComponent.h"

ATrainCargo::ATrainCargo()
{
	WoodComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WoodComp"));
	WoodComp->SetupAttachment(MeshComp);
	WoodComp->SetBoxExtent(FVector(50.0, 25.0, 50.0));
	WoodComp->SetRelativeLocation(FVector(0.0, -25.0, 0.0));

	IronComp = CreateDefaultSubobject<UBoxComponent>(TEXT("IronComp"));
	IronComp->SetupAttachment(MeshComp);
	IronComp->SetBoxExtent(FVector(50.0, 25.0, 50.0));
	IronComp->SetRelativeLocation(FVector(0.0, 25.0, 0.0));
}

void ATrainCargo::AddResource(class AActor* Resource)
{
	if (Resource->GetActorNameOrLabel().Contains("Wood")) {
		if (Woods.Num() == MaxCount)
			return;
		
		Woods.Add(Resource);
		return;
	}

	if (Irons.Num() == MaxCount)
		return;

	Irons.Add(Resource);
}
