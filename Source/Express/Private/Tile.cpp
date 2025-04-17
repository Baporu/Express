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
#include "SHS/GridManager.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	return temp; // �ٴ��� �ƴ� ��쿡�� ��Ȯ ����
}

void ATile::ReduceHP()
{
	if (TileType == ETileType::Ground || TileType == ETileType::Rock) // �ٴ��϶�
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
	//if(!CanHarvest()) // ��Ȯ�Ҽ� ������
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
	else if (TileType == ETileType::Water)
	{
		return;
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
	FVector SpawnLocation = CurrentTile->GetActorLocation();
	SpawnLocation.Z += 100;
	NewItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
	if (!NewItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Fail"));
	}
	else
	{
		NewItem->CreateItem(ItemType); // ������ ����
		CurrentTile->ContainedItem.Add(NewItem); // Ÿ�Ͽ� ������ �Ҵ�
	}


	Destroy(); // Ÿ�� �ı�
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
	else if (TileType == ETileType::Rock)
	{
		TileMesh->SetStaticMesh(RockMesh);
	}
	else if (TileType == ETileType::Water)
	{
		TileMesh->SetStaticMesh(WaterMesh);
	}
	else if (TileType == ETileType::Station_A)
	{
		TileMesh->SetStaticMesh(StationMesh);
		TileCollision->SetCollisionProfileName(TEXT("Item"));
	}
	else if (TileType == ETileType::Station_Z)
	{
		TileMesh->SetStaticMesh(StationMesh);
		TileCollision->SetCollisionProfileName(TEXT("Item"));
	}
}

void ATile::CreateTile(ETileType Type)
{
	TileType = Type;
	UpdateMeshMat();
	//UE_LOG(LogTemp, Warning, TEXT("CreateTile Success"));

}

bool ATile::CheckRail()
{
	if (!GridManager || GridManager->Grid.IsEmpty()) {
		UE_LOG(LogTemp, Fatal, TEXT("Error: Tile - CheckRail(), There is no grid."));
		return false;
	}

	// ���� Ÿ�� Ž��
	if (gridRow - 1 > 0) {
		// Ÿ�Ͽ� ���ΰ� ��������� ���� ���� ����
		if (GridManager->Grid[gridRow - 1][gridColumn]->TileType == ETileType::Rail)
			return true;
	}
	// ������ Ÿ�� Ž��
	if (gridRow + 1 < GridManager->Grid.Num()) {
		// Ÿ�Ͽ� ���ΰ� ��������� ���� ���� ����
		if (GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Rail)
			return true;
	}
	// ���� Ÿ�� Ž��
	if (gridColumn - 1 > 0) {
		// Ÿ�Ͽ� ���ΰ� ��������� ���� ���� ����
		if (GridManager->Grid[gridRow][gridColumn - 1]->TileType == ETileType::Rail)
			return true;
	}
	// �Ʒ��� Ÿ�� Ž��
	if (gridColumn + 1 < GridManager->Grid[gridRow].Num()) {
		// Ÿ�Ͽ� ���ΰ� ��������� ���� ���� ����
		if (GridManager->Grid[gridRow][gridColumn + 1]->TileType == ETileType::Rail)
			return true;
	}

	// ���� ���� �� ��������� ���� ���� �Ұ�
	return false;
}

void ATile::SetRail()
{
	TileType = ETileType::Rail;
}

