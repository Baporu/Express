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
    MeshComp->SetStaticMesh(nullptr); // 초기 메쉬 비우기
    MeshComp->SetMaterial(0, nullptr); // 초기 재질 비우기
    MeshComp->SetCollisionProfileName(TEXT("Item"));
	ItemType = EItemType::Wood;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> WoodMeshFinder(TEXT("/Game/SBS/MeshTex/Item_Wood.Item_Wood"));
    if (WoodMeshFinder.Succeeded())
    {
        WoodMesh = WoodMeshFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> StoneMeshFinder(TEXT("/Game/SBS/MeshTex/Item_Stone.Item_Stone"));
    if (StoneMeshFinder.Succeeded())
    {
        StoneMesh = StoneMeshFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> WoodMaterialFinder(TEXT("/Game/SBS/MeshTex/M_Wood.M_Wood"));
    if (WoodMaterialFinder.Succeeded())
    {
        WoodMaterial = WoodMaterialFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> StoneMaterialFinder(TEXT("/Game/SBS/MeshTex/M_Stone.M_Stone"));
    if (StoneMaterialFinder.Succeeded())
    {
        StoneMaterial = StoneMaterialFinder.Object;
    }
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
    }
    else if (ItemType == EItemType::Stone)
    {
        MeshComp->SetStaticMesh(StoneMesh);
    }

    //재질 설정
    if (ItemType == EItemType::Wood)
    {
        MeshComp->SetMaterial(0, WoodMaterial);
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
