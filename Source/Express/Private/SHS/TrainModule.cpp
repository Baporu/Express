// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainModule.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainEngine.h"
#include "Express/Express.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Tile.h"
#include "SBS/TileGenerator.h"
#include "SHS/GridManager.h"
#include "SBS/SBS_Player.h"

// Sets default values
ATrainModule::ATrainModule()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ModuleMeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetRelativeScale3D(FVector(1.0, 0.65, 0.8));

	ChainComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChainMeshComp"));
	ChainComp->SetupAttachment(RootComponent);
 	ChainComp->SetRelativeScale3D(FVector(0.5, 0.1, 0.1));
 	ChainComp->SetRelativeLocation(FVector(-75.0, 0.0, 0.0));

	ModuleComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ModuleComp"));
	ModuleComp->SetupAttachment(RootComponent);
	ModuleComp->SetBoxExtent(FVector(50.0));
	ModuleComp->SetRelativeLocation(FVector(-150.0, 0.0, 0.0));
	ModuleComp->SetCollisionProfileName(TEXT("NoCollision"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded()) {
		MeshComp->SetStaticMesh(tempMesh.Object);
		ChainComp->SetStaticMesh(tempMesh.Object);
	}

	WaterComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterComp"));
	WaterComp->SetupAttachment(RootComponent);
	WaterComp->SetBoxExtent(FVector(70.0, 50.0, 50.0));
	WaterComp->OnComponentBeginOverlap.AddDynamic(this, ATrainModule::OnWaterBeginOverlap);
}

// Called when the game starts or when spawned
void ATrainModule::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATrainModule::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTrain(DeltaTime);
	RotateTrain(DeltaTime);

	if (FVector::Dist2D(GetActorLocation(), NextPos) <= 0.5) {
		if (!TileQueue.IsEmpty())
			TileQueue.Dequeue(NextPos);

		if (!RotatorQueue.IsEmpty())
			RotatorQueue.Dequeue(NextRot);
	}
	

	// 시간 다 채우면 화재 시작
	if (FireTimer > FireTime) StartFire();

	// 불 붙은 상태면 화재 확산 시작
	if (bOnFire) OnFire(DeltaTime);
}

void ATrainModule::Init(ATrainEngine* EngineModule, float TrainSpeed, FVector Destination)
{
	TrainEngine = EngineModule;
	ModuleSpeed = TrainSpeed;
	NextPos = Destination;
}

void ATrainModule::SetModuleIndex(int32 ModuleIndex)
{
	ModuleNumber = ModuleIndex;
}

UBoxComponent* ATrainModule::GetModuleComp()
{
	return ModuleComp;
}

void ATrainModule::StartFire()
{
	// 이미 불 붙은 상태면 return
	if (bOnFire) return;

	bOnFire = true;

	// 화재 이펙트 소환
	FireComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, GetActorTransform());
}

void ATrainModule::EndFire()
{
	FireComp->Deactivate();

	bOnFire = false;
	FireTimer = 0.0f;
}

void ATrainModule::SetModuleLocation(FVector CurrentLocation)
{
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->GetBackModule(ModuleNumber)->SetModuleLocation(NextPos);

	TileQueue.Enqueue(CurrentLocation);

	UE_LOG(LogTrain, Log, TEXT("%s: Set Next Location - X: %f, Y: %f, Z: %f"), *GetActorNameOrLabel(), NextPos.X, NextPos.Y, NextPos.Z);

// 	// 현재 모듈이 가던 타일의 위치를 다음 모듈로 넘겨주고
// 	if (TrainEngine->CheckModule(ModuleNumber + 1))
// 		TrainEngine->GetBackModule(ModuleNumber)->SetModuleLocation(NextPos);
// 	
// 	// 이전 모듈이 넘겨준 타일의 위치를 다음 목적지로 설정
// 	NextPos = CurrentLocation;
}

void ATrainModule::SetModuleRotation(double CurrentYaw)
{
	// 현재 모듈의 회전값을 다음 모듈로 넘겨주고
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->GetBackModule(ModuleNumber)->SetModuleRotation(NextRot);
	
	// 이전 모듈이 넘겨준 회전값 받기
	RotatorQueue.Enqueue(CurrentYaw);
}

void ATrainModule::CheckNextTile()
{
	TArray<TArray<ATile*>> grid = GridManager->Grid;

	// 좌우 탐색
// 	if (RowIndex - 1 > 0)
// 		if (!grid[RowIndex - 1][ColIndex]->CheckContainedItem()) return;
// 
// 		if (grid[RowIndex - 1][ColIndex]->GetContainedItem()[0]->ItemType == EItemType::Rail && grid[RowIndex - 1][ColIndex]->bIsPassed == false) {
// 			CurrentTile = grid[RowIndex - 1][ColIndex];
// 
// 			NextPos = CurrentTile->GetActorLocation();
// 			// 왼쪽으로 가야 하니까 왼쪽으로 회전
// 			NextRot = 270.0;
// 
// 			RowIndex--;
// 			UE_LOG(LogTrain, Log, TEXT("Next Tile: Left"));
// 			return;
// 		}
// 	if (RowIndex + 1 < grid.Num())
// 		if (!grid[RowIndex + 1][ColIndex]->CheckContainedItem()) return;
// 
// 		if (grid[RowIndex + 1][ColIndex]->GetContainedItem()[0]->ItemType == EItemType::Rail && grid[RowIndex + 1][ColIndex]->bIsPassed == false) {
// 			CurrentTile = grid[RowIndex + 1][ColIndex];
// 
// 			NextPos = CurrentTile->GetActorLocation();
// 			// 오른쪽으로 회전
// 			NextRot = 90.0;
// 
// 			RowIndex++;
// 			UE_LOG(LogTrain, Log, TEXT("Next Tile: Right"));
// 			return;
// 		}
// 
// 	// 전후 탐색
// 	if (ColIndex - 1 > 0)
// 		if (!grid[RowIndex][ColIndex - 1]->CheckContainedItem()) return;
// 
// 		if (grid[RowIndex][ColIndex - 1]->GetContainedItem()[0]->ItemType == EItemType::Rail && grid[RowIndex][ColIndex - 1]->bIsPassed == false) {
// 			CurrentTile = grid[RowIndex][ColIndex - 1];
// 
// 			NextPos = CurrentTile->GetActorLocation();
// 			// 앞으로 회전
// 			NextRot = 0.0;
// 
// 			ColIndex--;
// 			UE_LOG(LogTrain, Log, TEXT("Next Tile: Up"));
// 			return;
// 		}
// 	if (ColIndex + 1 < grid[RowIndex].Num())
// 		if (!grid[RowIndex][ColIndex + 1]->CheckContainedItem()) return;
// 
// 		if (grid[RowIndex][ColIndex + 1]->GetContainedItem()[0]->ItemType == EItemType::Rail && grid[RowIndex][ColIndex - 1]->bIsPassed == false) {
// 			CurrentTile = grid[RowIndex][ColIndex + 1];
// 
// 			NextPos = CurrentTile->GetActorLocation();
// 			// 뒤로 회전
// 			NextRot = 180.0;
// 
// 			ColIndex++;
// 			UE_LOG(LogTrain, Log, TEXT("Next Tile: Down"));
// 			return;
// 		}

	// 좌우 탐색
	if (RowIndex - 1 > 0)
		if (grid[RowIndex - 1][ColIndex]->TileType == ETileType::Station_A && grid[RowIndex - 1][ColIndex]->bIsPassed == false) {
			CurrentTile = grid[RowIndex - 1][ColIndex];

			NextPos = CurrentTile->GetActorLocation();
			// 왼쪽으로 가야 하니까 왼쪽으로 회전
			NextRot = 270.0;

			RowIndex--;
			return;
		}
	if (RowIndex + 1 < grid.Num() && grid[RowIndex + 1][ColIndex]->bIsPassed == false)
		if (grid[RowIndex + 1][ColIndex]->TileType == ETileType::Station_A) {
			CurrentTile = grid[RowIndex + 1][ColIndex];
			
			NextPos = CurrentTile->GetActorLocation();
			// 오른쪽으로 회전
			NextRot = 90.0;

			RowIndex++;
			return;
		}
	
	// 전후 탐색
	if (ColIndex - 1 > 0)
		if (grid[RowIndex][ColIndex - 1]->TileType == ETileType::Station_A && grid[RowIndex][ColIndex - 1]->bIsPassed == false) {
			CurrentTile = grid[RowIndex][ColIndex - 1];

			NextPos = CurrentTile->GetActorLocation();
			// 앞으로 회전
			NextRot = 0.0;

			ColIndex--;
			return;
		}
	if (ColIndex + 1 < grid[RowIndex].Num() && grid[RowIndex][ColIndex + 1]->bIsPassed == false)
		if (grid[RowIndex][ColIndex + 1]->TileType == ETileType::Station_A) {
			CurrentTile = grid[RowIndex][ColIndex + 1];

			NextPos = CurrentTile->GetActorLocation();
			// 뒤로 회전
			NextRot = 180.0;

			ColIndex++;
			return;
		}
}

void ATrainModule::MoveTrain(float DeltaTime)
{
	FVector dir = NextPos - GetActorLocation();
	FVector vt = dir.GetSafeNormal() * ModuleSpeed * DeltaTime;
	SetActorLocation(GetActorLocation() + vt);
}

void ATrainModule::RotateTrain(float DeltaTime)
{
	double curRot = GetActorRotation().Yaw;
	curRot = FMath::Lerp(curRot, NextRot, DeltaTime);

	SetActorRotation(FRotator(0.0, curRot, 0.0));
}

void ATrainModule::OnFire(float DeltaTime)
{
	// 뒤에 모듈이 있는지 확인
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;

	// 엔진을 제외하면 앞에 모듈이 없을 수 없음
	TrainEngine->TrainModules[ModuleNumber - 1]->FireTimer += DeltaTime;
}

void ATrainModule::OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 불이 안 붙어있으면 return
	if (!bOnFire) return;

	ASBS_Player* player = Cast<ASBS_Player>(OtherActor);

	if (!player) return;

	player->bHasWater = false;
	EndFire();
}

