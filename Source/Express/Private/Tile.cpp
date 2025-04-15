// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Tile_FSM.h"
#include "Exp_GameMode.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Chaos/Vector.h"
#include "SBS/Item.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = TileMesh;
	TileFSM = CreateDefaultSubobject<UTile_FSM>(TEXT("Tile FSM"));
	TileType = ETileType::Ground;
	TileCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Tile Collision"));
	TileCollision->SetupAttachment(TileMesh);

	MaxTileHP = 3;
	CurTileHP = MaxTileHP;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	UpdateMeshMat();
}
//test
// Called every frame
//void ATile::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

bool ATile::CanHarvest() const
{
	bool temp = !(TileType == ETileType::Ground || TileType == ETileType::Rock);
	return temp; // 바닥이 아닌 경우에만 수확 가능
}

void ATile::ReduceHP()
{
	if (TileType == ETileType::Ground || TileType == ETileType::Rock) // 바닥일때
	{
		return;
	}
	CurTileHP--;
	if (CurTileHP <= 0)
	{
		HarvestTile();
	}
}

void ATile::HarvestTile()
{
	//if(!CanHarvest()) // 수확할수 없으면
	//{
	//	return;
	//}

	ATile* CurrentTile = nullptr;
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
	else
	{
		return;
	}
	FVector GroundLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f);
	for (TActorIterator<ATile> It(GetWorld()); It; ++It)
	{
		if (It->GetActorLocation() == GroundLocation && It->TileType == ETileType::Ground)
		{
			CurrentTile = *It;
			break;
		}
	}
	NewItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), CurrentTile->GetActorLocation(), FRotator::ZeroRotator);
	if (!NewItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spaw Item Fail"));
	}
	else
	{
		NewItem->CreateItem(ItemType); // 아이템 생성
		CurrentTile->ContainedItem.Add(NewItem); // 타일에 아이템 할당
	}


	Destroy(); // 타일 파괴
	CurTileHP = MaxTileHP;
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

