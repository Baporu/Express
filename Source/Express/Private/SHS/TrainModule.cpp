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
	

	// �ð� �� ä��� ȭ�� ����
	if (FireTimer > FireTime) StartFire();

	// �� ���� ���¸� ȭ�� Ȯ�� ����
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
	// �̹� �� ���� ���¸� return
	if (bOnFire) return;

	bOnFire = true;

	// ȭ�� ����Ʈ ��ȯ
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

// 	// ���� ����� ���� Ÿ���� ��ġ�� ���� ���� �Ѱ��ְ�
// 	if (TrainEngine->CheckModule(ModuleNumber + 1))
// 		TrainEngine->GetBackModule(ModuleNumber)->SetModuleLocation(NextPos);
// 	
// 	// ���� ����� �Ѱ��� Ÿ���� ��ġ�� ���� �������� ����
// 	NextPos = CurrentLocation;
}

void ATrainModule::SetModuleRotation(double CurrentYaw)
{
	// ���� ����� ȸ������ ���� ���� �Ѱ��ְ�
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->GetBackModule(ModuleNumber)->SetModuleRotation(NextRot);
	
	// ���� ����� �Ѱ��� ȸ���� �ޱ�
	RotatorQueue.Enqueue(CurrentYaw);
}

void ATrainModule::CheckNextTile()
{
	TArray<TArray<ATile*>> grid = GridManager->Grid;

	// �¿� Ž��
// 	if (RowIndex - 1 > 0)
// 		if (!grid[RowIndex - 1][ColIndex]->CheckContainedItem()) return;
// 
// 		if (grid[RowIndex - 1][ColIndex]->GetContainedItem()[0]->ItemType == EItemType::Rail && grid[RowIndex - 1][ColIndex]->bIsPassed == false) {
// 			CurrentTile = grid[RowIndex - 1][ColIndex];
// 
// 			NextPos = CurrentTile->GetActorLocation();
// 			// �������� ���� �ϴϱ� �������� ȸ��
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
// 			// ���������� ȸ��
// 			NextRot = 90.0;
// 
// 			RowIndex++;
// 			UE_LOG(LogTrain, Log, TEXT("Next Tile: Right"));
// 			return;
// 		}
// 
// 	// ���� Ž��
// 	if (ColIndex - 1 > 0)
// 		if (!grid[RowIndex][ColIndex - 1]->CheckContainedItem()) return;
// 
// 		if (grid[RowIndex][ColIndex - 1]->GetContainedItem()[0]->ItemType == EItemType::Rail && grid[RowIndex][ColIndex - 1]->bIsPassed == false) {
// 			CurrentTile = grid[RowIndex][ColIndex - 1];
// 
// 			NextPos = CurrentTile->GetActorLocation();
// 			// ������ ȸ��
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
// 			// �ڷ� ȸ��
// 			NextRot = 180.0;
// 
// 			ColIndex++;
// 			UE_LOG(LogTrain, Log, TEXT("Next Tile: Down"));
// 			return;
// 		}

	// �¿� Ž��
	if (RowIndex - 1 > 0)
		if (grid[RowIndex - 1][ColIndex]->TileType == ETileType::Station_A && grid[RowIndex - 1][ColIndex]->bIsPassed == false) {
			CurrentTile = grid[RowIndex - 1][ColIndex];

			NextPos = CurrentTile->GetActorLocation();
			// �������� ���� �ϴϱ� �������� ȸ��
			NextRot = 270.0;

			RowIndex--;
			return;
		}
	if (RowIndex + 1 < grid.Num() && grid[RowIndex + 1][ColIndex]->bIsPassed == false)
		if (grid[RowIndex + 1][ColIndex]->TileType == ETileType::Station_A) {
			CurrentTile = grid[RowIndex + 1][ColIndex];
			
			NextPos = CurrentTile->GetActorLocation();
			// ���������� ȸ��
			NextRot = 90.0;

			RowIndex++;
			return;
		}
	
	// ���� Ž��
	if (ColIndex - 1 > 0)
		if (grid[RowIndex][ColIndex - 1]->TileType == ETileType::Station_A && grid[RowIndex][ColIndex - 1]->bIsPassed == false) {
			CurrentTile = grid[RowIndex][ColIndex - 1];

			NextPos = CurrentTile->GetActorLocation();
			// ������ ȸ��
			NextRot = 0.0;

			ColIndex--;
			return;
		}
	if (ColIndex + 1 < grid[RowIndex].Num() && grid[RowIndex][ColIndex + 1]->bIsPassed == false)
		if (grid[RowIndex][ColIndex + 1]->TileType == ETileType::Station_A) {
			CurrentTile = grid[RowIndex][ColIndex + 1];

			NextPos = CurrentTile->GetActorLocation();
			// �ڷ� ȸ��
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
	// �ڿ� ����� �ִ��� Ȯ��
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;

	// ������ �����ϸ� �տ� ����� ���� �� ����
	TrainEngine->TrainModules[ModuleNumber - 1]->FireTimer += DeltaTime;
}

void ATrainModule::OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ���� �� �پ������� return
	if (!bOnFire) return;

	ASBS_Player* player = Cast<ASBS_Player>(OtherActor);

	if (!player) return;

	player->bHasWater = false;
	EndFire();
}

