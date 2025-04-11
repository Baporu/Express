// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Tile_FSM.h"
#include "Exp_GameMode.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Chaos/Vector.h"
#include "SBS/Item.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = TileMesh;
	TileFSM = CreateDefaultSubobject<UTile_FSM>(TEXT("Tile FSM"));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	
}
//test
// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CanHarvest() const
{
	return TileType != ETileType::Ground; // �ٴ��� �ƴ� ��쿡�� ��Ȯ ����
}

void ATile::HarvestTile()
{
	if(!CanHarvest() || !IsValid(this)) // ��Ȯ�Ҽ� ������
	{
		return;
	}
	// ETileType�� EItemType���� ��ȯ
	EItemType ItemType = (TileType == ETileType::Wood) ? EItemType::Wood : EItemType::Stone;

	FVector SpawnLocation = GetActorLocation();
	AItem* NewItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
	if (NewItem)
	{
		NewItem->CreateItem(ItemType, 1); // ������ ����
	}
	Destroy(); // Ÿ�� �ı�
}

