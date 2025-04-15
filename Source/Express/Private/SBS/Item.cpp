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
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Error, TEXT("MeshComp is null"));
        return;
    }

    // 1. 메쉬 설정
    UStaticMesh* SelectedMesh = nullptr;
    int32 MeshIndex = ItemStack - 1; // StackSize가 1일 때 인덱스 0

    if (ItemType == EItemType::Wood)
    {
        if (MeshIndex >= 0 && MeshIndex < WoodMeshes.Num())
        {
            SelectedMesh = WoodMeshes[MeshIndex];
        }
    }
    else if (ItemType == EItemType::Stone)
    {
        if (MeshIndex >= 0 && MeshIndex < StoneMeshes.Num())
        {
            SelectedMesh = StoneMeshes[MeshIndex];
        }
    }

    // 메쉬가 없으면 기본 메쉬 사용
    if (SelectedMesh)
    {
        MeshComp->SetStaticMesh(SelectedMesh);
    }
    else
    {
        MeshComp->SetStaticMesh(DefaultMesh);
        UE_LOG(LogTemp, Warning, TEXT("No valid mesh found for %s with StackSize %d. Using DefaultMesh."),
            *UEnum::GetValueAsString(ItemType), ItemStack);
    }

    // 2. 재질 설정
    UMaterialInterface* SelectedMaterial = nullptr;
    int32 MaterialIndex = static_cast<int32>(ItemType); // Wood=0, Stone=1

    if (MaterialIndex >= 0 && MaterialIndex < Materials.Num())
    {
        SelectedMaterial = Materials[MaterialIndex];
    }

    // 재질이 없으면 기본 재질 사용
    if (SelectedMaterial)
    {
        MeshComp->SetMaterial(0, SelectedMaterial);
    }
    else
    {
        MeshComp->SetMaterial(0, DefaultMaterial);
        UE_LOG(LogTemp, Warning, TEXT("No valid material found for %s. Using DefaultMaterial."),
            *UEnum::GetValueAsString(ItemType));
    }
    UE_LOG(LogTemp, Log, TEXT("WoodMeshes 크기: %d"), WoodMeshes.Num());
    UE_LOG(LogTemp, Log, TEXT("Materials 크기: %d"), Materials.Num());
    if (WoodMeshes.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("WoodMeshes[0]: %s"), WoodMeshes[0] ? *WoodMeshes[0]->GetName() : TEXT("null"));
    }
    if (Materials.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Materials[0]: %s"), Materials[0] ? *Materials[0]->GetName() : TEXT("null"));
    }
}

void AItem::CreateItem(EItemType Type, int StackSize)
{
	ItemType = Type;
	ItemStack = 1;
	UpdateMeshMat();
}

void AItem::SetStack(int StackSize)
{
	ItemStack = StackSize;
	UpdateMeshMat();
}

