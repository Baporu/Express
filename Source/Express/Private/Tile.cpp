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
#include "Exp_GameState.h"


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
	if (TileType == ETileType::Wood || TileType == ETileType::Stone || TileType ==ETileType::Rock)
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

void ATile::FindLastRail() {
	if (!GridManager || GridManager->Grid.IsEmpty())
		UE_LOG(LogTemp, Fatal, TEXT("Error: Tile - CheckRail(), There is no grid."));

	TArray<ATile*> Stack;
	Stack.Push(this);

	ATile* LastRailTile = this;

	while (Stack.Num() > 0) {
		ATile* Current = Stack.Pop();

		if (!Current || Current->bIsChecked)
			continue;

		Current->bIsChecked = true;
		Current->bIsLastRail = false;
		LastRailTile = Current;

		int32 Row = Current->gridRow;
		int32 Col = Current->gridColumn;

		// 오른쪽
		if (Row + 1 < GridManager->Grid.Num()) {
			ATile* Right = GridManager->Grid[Row + 1][Col];
			if (Right && Right->TileType == ETileType::Rail && !Right->bIsChecked)
				Stack.Push(Right);
		}

		// 왼쪽
		if (Row - 1 >= 0) {
			ATile* Left = GridManager->Grid[Row - 1][Col];
			if (Left && Left->TileType == ETileType::Rail && !Left->bIsChecked)
				Stack.Push(Left);
		}

		// 위쪽
		if (Col - 1 >= 0) {
			ATile* Up = GridManager->Grid[Row][Col - 1];
			if (Up && Up->TileType == ETileType::Rail && !Up->bIsChecked)
				Stack.Push(Up);
		}

		// 아래쪽
		if (Col + 1 < GridManager->Grid[Row].Num()) {
			ATile* Down = GridManager->Grid[Row][Col + 1];
			if (Down && Down->TileType == ETileType::Rail && !Down->bIsChecked)
				Stack.Push(Down);
		}
	}

	// 전부 순회한 뒤, 마지막 방문한 타일에만 표시
	if (LastRailTile)
		LastRailTile->bIsLastRail = true;
}

ATile* ATile::CheckRail()
{
	if (!GridManager || GridManager->Grid.IsEmpty()) {
		UE_LOG(LogTemp, Fatal, TEXT("Error: Tile - CheckRail(), There is no grid."));
		return nullptr;
	}

	// 왼쪽 타일 탐색, 타일에 선로가 깔려있으면 선로 연결 가능
	if (gridRow - 1 >= 0 && GridManager->Grid[gridRow - 1][gridColumn]->bIsLastRail) {
		// 오른쪽이 도착 역인지 확인
		if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}
		// 위쪽이 도착 역인지 확인
		else if (gridColumn - 1 >= 0 && GridManager->Grid[gridRow][gridColumn - 1]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}
		// 아래쪽이 도착 역인지 확인
		else if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && GridManager->Grid[gridRow][gridColumn + 1]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}

		return GridManager->Grid[gridRow - 1][gridColumn];
	}

	// 위쪽 타일 탐색
	if (gridColumn - 1 >= 0 && GridManager->Grid[gridRow][gridColumn - 1]->bIsLastRail) {
		// 오른쪽이 도착 역인지 확인
		if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}
		// 아래쪽이 도착 역인지 확인
		else if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && GridManager->Grid[gridRow][gridColumn + 1]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}

		return GridManager->Grid[gridRow][gridColumn - 1];
	}
	// 아래쪽 타일 탐색
	if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && GridManager->Grid[gridRow][gridColumn + 1]->bIsLastRail) {
		// 오른쪽이 도착 역인지 확인
		if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}
		// 위쪽이 도착 역인지 확인
		else if (gridColumn - 1 >= 0 && GridManager->Grid[gridRow][gridColumn - 1]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}

		return GridManager->Grid[gridRow][gridColumn + 1];
	}
	// 오른쪽 타일 탐색 (게임 구조 상 오른쪽 타일 검색할 일이 제일 적어서 아래로 뺌)
	if (gridRow + 1 < GridManager->Grid.Num() && GridManager->Grid[gridRow + 1][gridColumn]->bIsLastRail) {
		// 위쪽이 도착 역인지 확인
		if (gridColumn - 1 >= 0 && GridManager->Grid[gridRow][gridColumn - 1]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}
		// 아래쪽이 도착 역인지 확인
		else if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && GridManager->Grid[gridRow][gridColumn + 1]->TileType == ETileType::Station_Z) {
			Cast<AExp_GameState>(GetWorld()->GetGameState())->AccelTrain();
			bIsFinished = true;
		}

		return GridManager->Grid[gridRow + 1][gridColumn];
	}

	// 전부 선로 안 깔려있으면 선로 연결 불가
	return nullptr;
}


void ATile::SetRail(ATile* PreviousTile)
{
	TileType = ETileType::Rail;

	// 도착 역과 연결됐으면 게임을 끝냄
	//if (bIsFinished) return;

	// 이전 선로가 종단 선로였을 경우, 이 타일을 새로운 종단 선로로 설정함
	if (PreviousTile->bIsLastRail) {
		PreviousTile->bIsLastRail = false;
//		PreviousTile->TileCollision->SetCollisionProfileName(TEXT("Item"));
		bIsLastRail = true;
	}

	// 마지막 선로면서 열차가 지나가고 있지 않은 경우에만 선로를 주울 수 있음
// 	if (bIsLastRail && !bIsPassed)
// 		TileCollision->SetCollisionProfileName(TEXT("Tile"));
// 	else
// 		TileCollision->SetCollisionProfileName(TEXT("Item"));

	// 바닥에 연결 가능한 선로 아이템이 있는지 확인
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

	// 오른쪽 타일 탐색, 타일에 선로 아이템이 딱 하나 떨어져 있었다면 연결 가능
	if (gridRow + 1 < GridManager->Grid.Num() && !GridManager->Grid[gridRow + 1][gridColumn]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow + 1][gridColumn]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow + 1][gridColumn];

	// 아래쪽 타일 탐색
	if (gridColumn + 1 < GridManager->Grid[gridRow].Num() && !GridManager->Grid[gridRow][gridColumn + 1]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow][gridColumn + 1]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow][gridColumn + 1];

	// 위쪽 타일 탐색
	if (gridColumn - 1 >= 0 && !GridManager->Grid[gridRow][gridColumn - 1]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow][gridColumn - 1]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow][gridColumn - 1];

	// 왼쪽 타일 탐색 (게임 구조 상 가장 검색할 일이 적을 것 같아서 아래로 둠)
	if (gridRow - 1 >= 0 && !GridManager->Grid[gridRow - 1][gridColumn]->ContainedItem.IsEmpty() && GridManager->Grid[gridRow - 1][gridColumn]->ContainedItem.Num() == 1)
		return GridManager->Grid[gridRow - 1][gridColumn];

	// 전부 선로 안 깔려있으면 선로 연결 불가
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
		PRINTLOG(TEXT("Fix Here"));
		if (i > 0)
		{
			ContainedItem[i]->Server_Attach(ContainedItem[i - 1], FName(TEXT("ItemHead")));
		}
	}
}

void ATile::OnRep_TileType()
{
	if (TileType == ETileType::Rail) return;

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
	NewItem->CreateItem(ItemType); // 아이템 생성
	TArray<AItem*> TempItem;
	TempItem.Add(NewItem);
	CurrentTile->Server_SetContainedItem(TempItem);
	UE_LOG(LogTemp, Log, TEXT("HarvestTile: Item %s spawned on Tile %s"), *NewItem->GetName(), *CurrentTile->GetName());
	Destroy(); // 타일 파괴
}

void ATile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATile, ContainedItem);
	DOREPLIFETIME(ATile, TileType);

}

