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
	int MeshIndex = FMath::Clamp(ItemStack - 1, 0, 4); //�����۽��� 1~5�� 0~4 �ε����� ��ȯ
	int MaterialIndex = static_cast<int>(ItemType); //������Ÿ���� 0~1�ε����� ��ȯ

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

