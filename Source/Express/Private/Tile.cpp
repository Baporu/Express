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
#include "SHS/TrainEngine.h"
#include "Net/UnrealNetwork.h"
#include "Express/Express.h"


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
	bReplicates = true;
	//SetReplicates(true);

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
	Server_HarvestTile();
}

void ATile::UpdateMeshMat()
{
	switch (TileType)
	{
		case ETileType::Ground:
			TileMesh->SetStaticMesh(GroundMesh);
			//TileMesh->SetMaterial(0, GroundMat);
			break;
		case ETileType::Wood:
			TileMesh->SetStaticMesh(WoodMesh);
			break;
		case ETileType::Stone:
			TileMesh->SetStaticMesh(StoneMesh);
			break;
		case ETileType::Rock:
			TileMesh->SetStaticMesh(RockMesh);
			break;
		case ETileType::Rail:
			TileMesh->SetStaticMesh(RailMesh);
			TileCollision->SetCollisionProfileName(TEXT("Item"));
			break;
		case ETileType::Water:
			TileMesh->SetStaticMesh(WaterMesh);
			break;
		case ETileType::Station_A:
			TileMesh->SetStaticMesh(StationMesh);
			TileCollision->SetCollisionProfileName(TEXT("Item"));
			break;
		case ETileType::Station_Z:
			TileMesh->SetStaticMesh(StationMesh);
			TileCollision->SetCollisionProfileName(TEXT("Item"));
			break;
		default:
			UE_LOG(LogTrain, Warning, TEXT("Wrong TileType"));
			break;
	}
}

void ATile::CreateTile(ETileType Type)
{
	TileType = Type;
	if (TileType == ETileType::Wood || TileType == ETileType::Stone)
	{
		static const TArray<float> Rotations = { 90.0f, 180.0f, 270.0f };
		float RandRot = Rotations[FMath::RandHelper(Rotations.Num())];

		SetActorRotation(FRotator(0, RandRot, 0));
	}
	UpdateMeshMat();
}
void ATile::SetContainedItem(TArray<AItem*> Item)
{
	if (HasAuthority())
	{
		ContainedItem = Item;
		Multicast_SetContainedItem(Item);

	}
	else
	{
		Server_SetContainedItem(Item);
	}
}

ATile* ATile::CheckRail()
{
	if (!GridManager || GridManager->Grid.IsEmpty()) {
		UE_LOG(LogTemp, Fatal, TEXT("Error: Tile - CheckRail(), There is no grid."));
		return nullptr;
	}

	// ���� Ÿ�� Ž��, Ÿ�Ͽ� ���ΰ� ��������� ���� ���� ����
	if (gridRow - 1 > 0 && GridManager->Grid[gridRow - 1][gridColumn]->bIsLastRail) {
		// �������� ���� ������ Ȯ��
		if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Station_Z)
			bIsFinished = true;
		// ������ ���� ������ Ȯ��
		else if (gridColumn - 1 > 0 && GridManager->Grid[gridRow][gridColumn - 1]->TileType == ETileType::Station_Z)
			bIsFinished = true;
		// �Ʒ����� ���� ������ Ȯ��
		else if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && GridManager->Grid[gridRow][gridColumn + 1]->TileType == ETileType::Station_Z)
			bIsFinished = true;

		return GridManager->Grid[gridRow - 1][gridColumn];
	}

	// ���� Ÿ�� Ž��
	if (gridColumn - 1 > 0 && GridManager->Grid[gridRow][gridColumn - 1]->bIsLastRail) {
		// �������� ���� ������ Ȯ��
		if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Station_Z)
			bIsFinished = true;

		return GridManager->Grid[gridRow][gridColumn - 1];
	}
	// �Ʒ��� Ÿ�� Ž��
	if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && GridManager->Grid[gridRow][gridColumn + 1]->bIsLastRail) {
		// �������� ���� ������ Ȯ��
		if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Station_Z)
			bIsFinished = true;
		// ������ ���� ������ Ȯ��
		else if (gridColumn - 1 > 0 && GridManager->Grid[gridRow][gridColumn - 1]->TileType == ETileType::Station_Z)
			bIsFinished = true;

		return GridManager->Grid[gridRow][gridColumn + 1];
	}
	// ������ Ÿ�� Ž�� (���� ���� �� ������ Ÿ�� �˻��� ���� ���� ��� �Ʒ��� ��)
	if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->bIsLastRail) {
		// ���� ���� �����ʿ��� ������ �� ����
		return GridManager->Grid[gridRow + 1][gridColumn];
	}

	// ���� ���� �� ��������� ���� ���� �Ұ�
	return nullptr;
}


void ATile::SetRail(ATile* PreviousTile)
{
	TileType = ETileType::Rail;

	// ���� ���� ��������� ������ ����
	//if (bIsFinished) return;

	// ���� ���ΰ� ���� ���ο��� ���, �� Ÿ���� ���ο� ���� ���η� ������
	if (PreviousTile->bIsLastRail) {
		PreviousTile->bIsLastRail = false;
//		PreviousTile->TileCollision->SetCollisionProfileName(TEXT("Item"));
		bIsLastRail = true;
	}

	// ������ ���θ鼭 ������ �������� ���� ���� ��쿡�� ���θ� �ֿ� �� ����
// 	if (bIsLastRail && !bIsPassed)
// 		TileCollision->SetCollisionProfileName(TEXT("Tile"));
// 	else
// 		TileCollision->SetCollisionProfileName(TEXT("Item"));

	// �ٴڿ� ���� ������ ���� �������� �ִ��� Ȯ��
	if (ATile* NextTile = CheckRailItem()) {
		NextTile->ContainedItem.Empty();
		NextTile->SetRail(this);
	}
}

ATile* ATile::CheckRailItem()
{
	if (!GridManager || GridManager->Grid.IsEmpty()) {
		PRINTFATALLOG(TEXT("There is no grid."));
		return nullptr;
	}

	// ������ Ÿ�� Ž��, Ÿ�Ͽ� ���� �������� �� �ϳ� ������ �־��ٸ� ���� ����
	if (gridRow + 1 < GridManager->Grid.Num() && !GridManager->Grid[gridRow + 1][gridColumn]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow + 1][gridColumn]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow + 1][gridColumn];

	// �Ʒ��� Ÿ�� Ž��
	if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && !GridManager->Grid[gridRow][gridColumn + 1]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow][gridColumn + 1]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow][gridColumn + 1];

	// ���� Ÿ�� Ž��
	if (gridColumn - 1 > 0 && !GridManager->Grid[gridRow][gridColumn - 1]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow][gridColumn - 1]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow][gridColumn - 1];

	// ���� Ÿ�� Ž�� (���� ���� �� ���� �˻��� ���� ���� �� ���Ƽ� �Ʒ��� ��)
	if (gridRow - 1 > 0 && !GridManager->Grid[gridRow - 1][gridColumn]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow - 1][gridColumn]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow - 1][gridColumn];

	// ���� ���� �� ��������� ���� ���� �Ұ�
	return nullptr;
}

void ATile::Server_SetContainedItem_Implementation(const TArray<AItem*>& Item)
{
	ContainedItem = Item;
	Multicast_SetContainedItem(Item);
	UE_LOG(LogTemp, Warning, TEXT("Server: Set ContainedItem, Count = %d"), Item.Num());
	//OnRep_ContainedItem();
}

void ATile::Server_SetItemLocation_Implementation(const FVector& NewLocation)
{
	SetActorLocation(NewLocation);
}

void ATile::Multicast_SetContainedItem_Implementation(const TArray<AItem*>& Item)
{
	ContainedItem = Item;
	OnRep_ContainedItem();
}

void ATile::OnRep_ContainedItem()
{
	if (ContainedItem.IsEmpty())
	{
		
		//ContainedItem.Empty();
		UE_LOG(LogTemp, Warning, TEXT("OnRep_ContainedItem: Item is null"));

		return;
	}

	FVector BaseLocation = GetActorLocation() + FVector(0, 0, 100);
	for (int i = 0; i < ContainedItem.Num(); i++)
	{
		if (!ContainedItem[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("Client: ContainedItem cleared"));
			continue;
		}

		ContainedItem[i]->Server_Detach();
		if (i > 0)
		{
			ContainedItem[i]->Server_Attach(ContainedItem[i - 1], FName(TEXT("ItemHead")));
		}
	}
}

void ATile::OnRep_TileType()
{
	UpdateMeshMat();
}

void ATile::Server_SetRail_Implementation(ATile* PreviousTile)
{
	SetRail(PreviousTile);
}

void ATile::Server_HarvestTile_Implementation()
{
	ATile* CurrentTile = nullptr;
	AItem* NewItem;

	EItemType ItemType = EItemType::Wood;
	if (TileType == ETileType::Wood)
	{
		ItemType = EItemType::Wood;
	}
	else if (TileType == ETileType::Stone)
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
		if (It->GetActorLocation() == GroundLocation)// && It->TileType == ETileType::Ground)
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
		return;
	}
	NewItem->SetReplicates(true);
	NewItem->ForceNetUpdate();
	NewItem->CreateItem(ItemType); // ������ ����
	TArray<AItem*> TempItem;
	TempItem.Add(NewItem);
	CurrentTile->Server_SetContainedItem(TempItem);
	UE_LOG(LogTemp, Log, TEXT("HarvestTile: Item %s spawned on Tile %s"), *NewItem->GetName(), *CurrentTile->GetName());
	Destroy(); // Ÿ�� �ı�
}

void ATile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATile, ContainedItem);
	DOREPLIFETIME(ATile, TileType);

}

