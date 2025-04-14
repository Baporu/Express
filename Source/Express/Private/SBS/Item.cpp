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
	ItemStack = 1;

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
	TArray<UStaticMesh*>* TargetMeshes;
	int MeshIndex = FMath::Clamp(ItemStack - 1, 0, 4); //아이템스텍 1~5를 0~4 인덱스로 변환
	int MaterialIndex = static_cast<int>(ItemType); //아이템타입을 0~1인덱스로 변환

	if (ItemType == EItemType::Wood)
	{
		TargetMeshes = &WoodMeshes;
	}
	else
	{
		TargetMeshes = &StoneMeshes;
	}

	MeshComp->SetStaticMesh((*TargetMeshes)[MeshIndex]);
	MeshComp->SetMaterial(0, Materials[MaterialIndex]);
}

void AItem::CreateItem(EItemType Type, int StackSize)
{
	ItemType = Type;
	ItemStack = StackSize;
	UpdateMeshMat();
}

void AItem::SetStack(int StackSize)
{
	ItemStack = StackSize;
	UpdateMeshMat();
}

