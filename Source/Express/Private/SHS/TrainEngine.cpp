// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainEngine.h"
#include "Express/Express.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainWaterTank.h"
#include "SHS/TrainCargo.h"
#include "SHS/TrainCrafter.h"
#include "Tile.h"
#include "SHS/GridManager.h"
#include "SBS/SBS_Player.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATrainEngine::ATrainEngine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATrainEngine::BeginPlay()
{
	Super::BeginPlay();

	TrainEngine = this;
	ModuleNumber = 0;

	if (HasAuthority()) {
		TrainModules.Add(this);
		SpawnDefaultModules();
	}
}

// Called every frame
void ATrainEngine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 서버에서만 실행
	if (!HasAuthority()) return;

	if (!bIsStarted) return;

	// 타일 위치에 도달했을 경우, 다음 타일 검색
	if (FVector::Dist2D(GetActorLocation(), NextPos) <= 0.5) {
		if (CurrentTile->bIsFinished) { PRINTFATALLOG(TEXT("GAME CLEAR!")); }
		else { CheckNextTile(); }
	}
}

void ATrainEngine::Init(AGridManager* Grid, ATile* NextTile, int32 Row, int32 Column)
{
	GridManager = Grid;
	CurrentTile = NextTile;
	RowIndex = Row;
	ColIndex = Column;

	NextPos = CurrentTile->GetActorLocation();
	
	FVector CurPos = GetActorLocation();

	if (NextPos.Y >= CurPos.Y)
		NextRot = 90.0;
	else if (NextPos.Y < CurPos.Y)
		NextRot = 270.0;
	else if (NextPos.X >= CurPos.X)
		NextRot = 0.0;
	else NextRot = 180.0;

	FTimerHandle InitHandle;
	GetWorld()->GetTimerManager().SetTimer(InitHandle, FTimerDelegate::CreateLambda([&]
																					{
																						CheckNextTile();
																						for (int i = 0; i < TrainModules.Num(); ++i)
																							TrainModules[i]->bIsStarted = true;
																					}), InitTime, false);
}

bool ATrainEngine::CheckModule(int32 ModuleIndex)
{
	if (TrainModules.Num() == ModuleIndex)
		return false;

	return true;
}

ATrainModule* ATrainEngine::GetFrontModule(int32 CurrentModuleIndex)
{
	if (CurrentModuleIndex < 1)
		return nullptr;

	return TrainModules[CurrentModuleIndex - 1];
}

ATrainModule* ATrainEngine::GetBackModule(int32 CurrentModuleIndex)
{
	if (TrainModules.Num() == CurrentModuleIndex + 1)
		return nullptr;

	return TrainModules[CurrentModuleIndex + 1];
}

void ATrainEngine::AttachModule(ATrainModule* TrainModule)
{
//	TrainModule->AttachToComponent(ModuleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TrainModule->SetActorLocation(ModuleComp->GetComponentLocation());
	TrainModule->SetActorRotation(GetActorRotation());
	TrainModules.Add(TrainModule);
}

void ATrainEngine::AttachModule(ATrainModule* TrainModule, int32 AttachIndex)
{
	USceneComponent* AttachComp = TrainModules[AttachIndex]->GetModuleComp();

//	TrainModule->AttachToComponent(AttachComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TrainModule->SetActorLocation(AttachComp->GetComponentLocation());
	TrainModule->SetActorRotation(GetActorRotation());
	TrainModules.Add(TrainModule);
}

void ATrainEngine::AddFireTime(float WaterTankTime)
{
	FireTime += WaterTankTime;
	UE_LOG(LogTrain, Log, TEXT("Fire Time Added"));
}

void ATrainEngine::CheckMakeRail()
{
	for (int32 i = 0; i < TrainModules.Num(); i++)
		if (ATrainCrafter* crafter = Cast<ATrainCrafter>(TrainModules[i]))
			crafter->CheckMakeRail();
}

void ATrainEngine::CheckNextTile()
{
	if (!HasAuthority()) return;

	if (!GridManager) {
		PRINTFATALLOG(TEXT("There is no GridManager."));
		return;
	}

	TArray<TArray<ATile*>> grid = GridManager->Grid;
	
	if (grid.IsEmpty()) {
		PRINTFATALLOG(TEXT("There is no grid."));
		return;
	}

//	PRINTLOG(TEXT("Tile Check Started"));

	// 현재 위치를 다음 모듈에게 넘겨주고
	TrainModules[1]->SetModuleLocation(NextPos);
	TrainModules[1]->SetModuleRotation(NextRot);

//	PRINTLOG(TEXT("Location Enqueued"));

	// 상하 탐색
	if (RowIndex - 1 > 0)
		// 선로가 깔린 타일이고, 아직 지나가지 않은 길이면
		if (grid[RowIndex - 1][ColIndex]->TileType == ETileType::Rail && grid[RowIndex - 1][ColIndex]->bIsPassed == false) {
			CurrentTile = grid[RowIndex - 1][ColIndex];

			NextPos = CurrentTile->GetActorLocation();
			CurrentTile->bIsPassed = true;
			NextPos.Z += 115.0;
			// 위로 가야 하니까 위쪽으로 회전
			NextRot = 0.0;

			RowIndex--;
			return;
		}
	if (RowIndex + 1 < grid.Num())
		if (grid[RowIndex + 1][ColIndex]->TileType == ETileType::Rail && grid[RowIndex + 1][ColIndex]->bIsPassed == false) {
			CurrentTile = grid[RowIndex + 1][ColIndex];

			NextPos = CurrentTile->GetActorLocation();
			CurrentTile->bIsPassed = true;
			NextPos.Z += 115.0;
			// 아래쪽으로 회전
			NextRot = 180.0;

			RowIndex++;
			return;
		}

	// 좌우 탐색
	if (ColIndex - 1 > 0)
		if (grid[RowIndex][ColIndex - 1]->TileType == ETileType::Rail && grid[RowIndex][ColIndex - 1]->bIsPassed == false) {
			CurrentTile = grid[RowIndex][ColIndex - 1];

			NextPos = CurrentTile->GetActorLocation();
			CurrentTile->bIsPassed = true;
			NextPos.Z += 115.0;
			// 왼쪽으로 회전
			NextRot = 270.0;

			ColIndex--;
			return;
		}
	if (ColIndex + 1 < grid[RowIndex].Num())
		if (grid[RowIndex][ColIndex + 1]->TileType == ETileType::Rail && grid[RowIndex][ColIndex + 1]->bIsPassed == false) {
			CurrentTile = grid[RowIndex][ColIndex + 1];

			NextPos = CurrentTile->GetActorLocation();
			CurrentTile->bIsPassed = true;
			NextPos.Z += 115.0;
			// 오른쪽으로 회전
			NextRot = 90.0;

			ColIndex++;
			return;
		}

	//PRINTFATALLOG(TEXT("Current Rail: %s, There is no rail, Game Failed"), *CurrentTile->GetActorNameOrLabel());
}

void ATrainEngine::MoveTrain(float DeltaTime)
{
	FVector dir = NextPos - GetActorLocation();
	FVector vt = dir.GetSafeNormal() * TrainSpeed * DeltaTime;
	SetActorLocation(GetActorLocation() + vt);
}

void ATrainEngine::OnFire(float DeltaTime)
{
	TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;
}

void ATrainEngine::OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 엔진도 물탱크도 불 안 붙었으면 return
	if (!bOnFire && (TrainWaterTank && !TrainWaterTank->bOnFire)) return;

	ASBS_Player* player = Cast<ASBS_Player>(OtherActor);
	if (!player || !player->IsLocallyControlled()) return;

	if (player->HoldItems.IsEmpty() || player->HoldItems[0]->IsBucketEmpty) return;

	// 엔진에 불 붙은 경우
	if (bOnFire) player->Client_EndFire(this, player);

	// 물탱크에 불 붙은 경우
	if (TrainWaterTank->bOnFire)
		player->Client_EndFire(TrainWaterTank, player);
}

void ATrainEngine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATrainEngine, TrainModules);
	DOREPLIFETIME(ATrainEngine, GridManager);
	DOREPLIFETIME(ATrainEngine, TrainWaterTank);
}

// ================================ 임시 함수들 ================================
void ATrainEngine::GetTileLocation()
{	
	// 현재 위치를 다음 모듈에게 넘겨주고
	TrainModules[1]->SetModuleLocation(NextPos);
	TrainModules[1]->SetModuleRotation(NextRot);

	int rand = FMath::RandRange(1, 4);

	switch (rand)
	{
		// 전
		case 1:
			NextPos.X += 100.0f;
			NextRot = 0.0;
			break;
		// 후
		case 2:
			NextPos.X -= 100.0f;
			NextRot = 180.0;
			break;
		// 좌
		case 3:
			NextPos.Y += 100.0f;
			NextRot = 90.0;
			break;
		// 우
		case 4:
			NextPos.Y -= 100.0f;
			NextRot = -90.0;
			break;
	}

	UE_LOG(LogTrain, Log, TEXT("Next Position Changed"));
}

void ATrainEngine::SpawnDefaultModules()
{
	ATrainWaterTank* WaterTank = GetWorld()->SpawnActorDeferred<ATrainWaterTank>(BP_WaterTank, GetActorTransform());
	WaterTank->Init(this, TrainSpeed, GetActorLocation());
	WaterTank->FinishSpawning(GetActorTransform());

	ATrainCargo* Cargo = GetWorld()->SpawnActorDeferred<ATrainCargo>(BP_Cargo, GetActorTransform());
	Cargo->Init(this, TrainSpeed, GetActorLocation());
	Cargo->FinishSpawning(GetActorTransform());

	ATrainCrafter* Crafter = GetWorld()->SpawnActorDeferred<ATrainCrafter>(BP_Crafter, GetActorTransform());
	Crafter->Init(this, TrainSpeed, GetActorLocation(), Cargo);
	Crafter->FinishSpawning(GetActorTransform());

	AttachModule(WaterTank);
	WaterTank->SetModuleIndex(1);
	TrainWaterTank = WaterTank;
	AttachModule(Cargo, 1);
	Cargo->SetModuleIndex(2);
	AttachModule(Crafter, 2);
	Crafter->SetModuleIndex(3);
}

