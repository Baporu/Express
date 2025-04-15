// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/Item.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ItemType = EItemType::Wood;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	UpdateMeshMat();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::UpdateMeshMat()
{
   
    if (ItemType == EItemType::Wood)
    {
        MeshComp->SetStaticMesh(WoodMesh);

        UE_LOG(LogTemp, Warning, TEXT("UpdateMesh Success"));

    }
    else if (ItemType == EItemType::Stone)
    {
        MeshComp->SetStaticMesh(StoneMesh);
    }

    //재질 설정
    if (ItemType == EItemType::Wood)
    {
        MeshComp->SetMaterial(0, WoodMaterial);
        UE_LOG(LogTemp, Warning, TEXT("UpdateMaterial Success"));

    }
    else if (ItemType == EItemType::Stone)
    {
        MeshComp->SetMaterial(0, StoneMaterial);
    }
    
}
void AItem::CreateItem(EItemType Type)
{
	ItemType = Type;
	UpdateMeshMat();
}

void AItem::StackItem()
{
    //AttachToActor()
}
