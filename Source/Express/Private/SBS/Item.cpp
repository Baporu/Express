// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true);
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
    MeshComp->SetStaticMesh(nullptr); // 초기 메쉬 비우기
    MeshComp->SetMaterial(0, nullptr); // 초기 재질 비우기
    MeshComp->SetCollisionProfileName(TEXT("Item"));
	ItemType = EItemType::Wood;
    //Mesh 설정
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
    static ConstructorHelpers::FObjectFinder<UStaticMesh> RailMeshFinder(TEXT("/Game/SBS/MeshTex/Item_Rail.Item_Rail"));
    if (StoneMeshFinder.Succeeded())
    {
        RailMesh = RailMeshFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AxeMeshFinder(TEXT("/Game/SBS/MeshTex/Item_Axe.Item_Axe"));
    if (StoneMeshFinder.Succeeded())
    {
        AxeMesh = AxeMeshFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PickaxeMeshFinder(TEXT("/Game/SBS/MeshTex/Item_Pickaxe.Item_Pickaxe"));
    if (StoneMeshFinder.Succeeded())
    {
        PickaxeMesh = PickaxeMeshFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BucketMeshFinder(TEXT("/Game/SBS/MeshTex/Item_Bucket.Item_Bucket"));
    if (StoneMeshFinder.Succeeded())
    {
        BucketMesh = BucketMeshFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BucketMesh_EmptyFinder(TEXT("/Game/SBS/MeshTex/Item_Bucket_Empty.Item_Bucket_Empty"));
    if (StoneMeshFinder.Succeeded())
    {
        BucketMesh_Empty = BucketMesh_EmptyFinder.Object;
    }

    //Material 설정
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
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RailMaterialFinder(TEXT("/Game/SBS/MeshTex/M_Rail.M_Rail"));
    if (StoneMaterialFinder.Succeeded())
    {
        RailMaterial = RailMaterialFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> AxeMaterialFinder(TEXT("/Game/SBS/MeshTex/M_Axe.M_Axe"));
    if (StoneMaterialFinder.Succeeded())
    {
        AxeMaterial = AxeMaterialFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> PickaxeMaterialFinder(TEXT("/Game/SBS/MeshTex/M_Pickaxe.M_Pickaxe"));
    if (StoneMaterialFinder.Succeeded())
    {
        PickaxeMaterial = PickaxeMaterialFinder.Object;
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BucketMaterialFinder(TEXT("/Game/SBS/MeshTex/M_Bucket.M_Bucket"));
    if (StoneMaterialFinder.Succeeded())
    {
        BucketMaterial = BucketMaterialFinder.Object;
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
    else if (ItemType == EItemType::Rail)
    {
        MeshComp->SetStaticMesh(RailMesh);
    }
    else if (ItemType == EItemType::Axe)
    {
        MeshComp->SetStaticMesh(AxeMesh);
    }
    else if (ItemType == EItemType::Pickaxe)
    {
        MeshComp->SetStaticMesh(PickaxeMesh);
    }
    else if (ItemType == EItemType::Bucket && !IsBucketEmpty)
    {
        MeshComp->SetStaticMesh(BucketMesh);
    }
    else if (ItemType == EItemType::Bucket && IsBucketEmpty)
    {
        MeshComp->SetStaticMesh(BucketMesh_Empty);
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
    else if (ItemType == EItemType::Rail)
    {
        MeshComp->SetMaterial(0, RailMaterial);
    }
    else if (ItemType == EItemType::Axe)
    {
        MeshComp->SetMaterial(0, AxeMaterial);
    }
    else if (ItemType == EItemType::Pickaxe)
    {
        MeshComp->SetMaterial(0, PickaxeMaterial);
    }
    else if (ItemType == EItemType::Bucket)
    {
        MeshComp->SetMaterial(0, BucketMaterial);
    }
    
}
void AItem::CreateItem(EItemType Type)
{
    //서버면
    if (HasAuthority())
    {
		ItemType = Type;
		if (ItemType == EItemType::Axe || ItemType == EItemType::Pickaxe || ItemType == EItemType::Bucket)
			IsTool = true;
		UpdateMeshMat();
		Multicast_UpdateMeshMat();
    }
    //클라면
    else
    {
        Server_CreateItem(Type);//서버에 전달
    }
}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AItem, ItemType);
}

void AItem::OnRep_ItemType()
{
    UpdateMeshMat();
}

void AItem::Server_CreateItem_Implementation(EItemType Type)
{
	ItemType = Type;
	if (ItemType == EItemType::Axe || ItemType == EItemType::Pickaxe || ItemType == EItemType::Bucket)
		IsTool = true;
	UpdateMeshMat();
	Multicast_UpdateMeshMat();
}
void AItem::Multicast_UpdateMeshMat_Implementation()
{
    UpdateMeshMat();
}

