// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Tile_FSM.h"
#include "Exp_GameMode.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Chaos/Vector.h"
#include "SBS/Item.h"
#include "EngineUtils.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = TileMesh;
	TileFSM = CreateDefaultSubobject<UTile_FSM>(TEXT("Tile FSM"));
	TileType = ETileType::Ground;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	UpdateMeshMat();
}
//test
// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UpdateMeshMat();
	if (bTrigger)
	{
		HarvestTile();
		//Destroy();
	}
}

bool ATile::CanHarvest() const
{
	return TileType != ETileType::Ground; // 바닥이 아닌 경우에만 수확 가능
}

void ATile::HarvestTile()
{
	if(!CanHarvest() || !IsValid(this)) // 수확할수 없으면
	{
		return;
	}

	ATile* GroundTile = nullptr;
	AItem* NewItem;

	EItemType ItemType = EItemType::Wood;
	if (TileType == ETileType::Wood)
	{
		ItemType = EItemType::Wood;
	}
	else if(TileType == ETileType::Stone)
	{
		ItemType = EItemType::Stone;
	}


	FVector GroundLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f);
	for (TActorIterator<ATile> It(GetWorld()); It; ++It)
	{
		if (It->GetActorLocation() == GroundLocation && It->TileType == ETileType::Ground)
		{
			GroundTile = *It;
			break;
		}
	}
	NewItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), GroundTile->GetActorLocation(), FRotator::ZeroRotator);
	if (NewItem)
	{
		NewItem->CreateItem(ItemType, 1); // 아이템 생성
		GroundTile->ContainedItem = NewItem; // 타일에 아이템 할당
	}
	Destroy(); // 타일 파괴
}

void ATile::UpdateMeshMat()
{

	if (TileType == ETileType::Ground)
	{
		TileMesh->SetStaticMesh(GroundMesh);
		//TileMesh->SetMaterial(0, GroundMat);
	}
	else if(TileType == ETileType::Stone)
	{
		TileMesh->SetStaticMesh(StoneMesh);
		//TileMesh->SetMaterial(0, StoneMat);
	}
	else if (TileType == ETileType::Wood)
	{
		TileMesh->SetStaticMesh(WoodMesh);
		//TileMesh->SetMaterial(0, WoodMat);
	}
}

void ATile::CreateTile(ETileType Type)
{
	TileType = Type;
	UpdateMeshMat();
}

