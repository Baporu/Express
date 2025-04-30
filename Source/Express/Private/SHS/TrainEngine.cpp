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
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
#include "Exp_GameState.h"
#include "Kismet/GameplayStatics.h"

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
	MinSpeed = TrainSpeed;

	if (HasAuthority()) {
		Cast<AExp_GameState>(GetWorld()->GetGameState())->SetTrainEngine(this);
		GetMainCamera();
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

	if (bIsCleared) return;

	if (bIsFinished) bOnAccel ? AccelModules(DeltaTime) : DecelModules(DeltaTime);

	AddTrainSpeed(DeltaTime);

	// 타일 위치에 도달했을 경우, 다음 타일 검색
	if (FVector::Dist2D(GetActorLocation(), NextPos) <= 0.5) {
		CheckNextTile();
	}
}

void ATrainEngine::Init(AGridManager* Grid, ATile* NextTile, int32 Row, int32 Column)
{
	GridManager = Grid;
	CurrentTile = NextTile;
	RowIndex = Row;
	ColIndex = Column;

	NextPos = GetActorLocation();
	
	FVector CurPos = GetActorLocation();

	if (NextPos.Y >= CurPos.Y)
		NextRot = 90.0;
	else if (NextPos.Y < CurPos.Y)
		NextRot = 270.0;
	else if (NextPos.X >= CurPos.X)
		NextRot = 0.0;
	else NextRot = 180.0;

	auto gs = Cast<AExp_GameState>(GetWorld()->GetGameState());

	if (!gs) return;

	gs->Server_CheckLoading();
}

void ATrainEngine::SetInitTimer() {
	FTimerHandle InitHandle;
	GetWorld()->GetTimerManager().SetTimer(InitHandle, FTimerDelegate::CreateLambda([&]
																					{
																						CheckNextTile();
																						for (int i = 0; i < TrainModules.Num(); ++i)
																							TrainModules[i]->bIsStarted = true;
																						UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound_TrainStart, GetActorLocation());

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

void ATrainEngine::AccelModules() {
	for (int i = 0; i < TrainModules.Num(); ++i)
		TrainModules[i]->bIsFinished = true;
}

void ATrainEngine::DecelModules() {
	bOnAccel = false;
	EasingAlpha = 0.f;
}

bool ATrainEngine::GetIsStarted() {
	return bIsStarted;
}

void ATrainEngine::CheckNextTile()
{
	if (!HasAuthority()) return;

	if (bIsCleared) return;

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

	if (CurrentTile->bIsFinished) {
		bOnAccel = false;
		EasingAlpha = 0.0f;
	}

	// 상하 탐색
	if (RowIndex - 1 > 0)
		// 선로가 깔린 타일이고, 아직 지나가지 않은 길이면
		if ((grid[RowIndex - 1][ColIndex]->TileType == ETileType::Rail && grid[RowIndex - 1][ColIndex]->bIsPassed == false) || grid[RowIndex - 1][ColIndex]->TileType == ETileType::Station_Z) {
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
		if ((grid[RowIndex + 1][ColIndex]->TileType == ETileType::Rail && grid[RowIndex + 1][ColIndex]->bIsPassed == false) || grid[RowIndex + 1][ColIndex]->TileType == ETileType::Station_Z) {
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
		if ((grid[RowIndex][ColIndex - 1]->TileType == ETileType::Rail && grid[RowIndex][ColIndex - 1]->bIsPassed == false) || grid[RowIndex][ColIndex - 1]->TileType == ETileType::Station_Z) {
			CurrentTile = grid[RowIndex][ColIndex - 1];

			NextPos = CurrentTile->GetActorLocation();
			CurrentTile->bIsPassed = true;
			NextPos.Z += 115.0;
			// 왼쪽으로 회전
			NextRot = -90.0;

			ColIndex--;
			return;
		}
	if (ColIndex + 1 < grid[RowIndex].Num())
		if ((grid[RowIndex][ColIndex + 1]->TileType == ETileType::Rail && grid[RowIndex][ColIndex + 1]->bIsPassed == false) || grid[RowIndex][ColIndex + 1]->TileType == ETileType::Station_Z) {
			CurrentTile = grid[RowIndex][ColIndex + 1];

			NextPos = CurrentTile->GetActorLocation();
			CurrentTile->bIsPassed = true;
			NextPos.Z += 115.0;
			// 오른쪽으로 회전
			NextRot = 90.0;

			ColIndex++;
			return;
		}

	auto gs = Cast<AExp_GameState>(GetWorld()->GetGameState());
	if (!gs) PRINTFATALLOG(TEXT("There isn't the appropriate game state."));

	bIsCleared = true;

	if (CurrentTile->TileType == ETileType::Station_Z) {
		gs->bIsGameCleared = true;
		gs->OnServer_GameEnd(true);
	}
	else {
		gs->bIsGameFailed = true;
		gs->OnServer_GameEnd(false);
	}
}

void ATrainEngine::MoveTrain(float DeltaTime)
{
	FVector dir = NextPos - GetActorLocation();
	FVector vt = dir.GetSafeNormal() * TrainSpeed * DeltaTime;
	SetActorLocation(GetActorLocation() + vt);

	// 목표 위치에 도달하거나 지나칠 경우
	if (FVector::Dist2D(GetActorLocation(), NextPos) <= vt.Size())
		// 목표 위치에 정확하게 도달하도록 강제로 설정
		SetActorLocation(NextPos);
	else
		// 목표 위치에 도달하지 않았다면 계속 이동
		SetActorLocation(GetActorLocation() + vt);

	// 카메라도 같이 이동
	MainCam->SetActorLocation(MainCam->GetActorLocation() + FVector(0.0, vt.Y * 1.5, 0.0));
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
	if (bOnFire) player->Server_RequestEndFire(this);

	// 물탱크에 불 붙은 경우
	if (TrainWaterTank->bOnFire)
		player->Server_RequestEndFire(TrainWaterTank);
}

void ATrainEngine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATrainEngine, TrainModules);
	DOREPLIFETIME(ATrainEngine, GridManager);
	DOREPLIFETIME(ATrainEngine, TrainWaterTank);
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

void ATrainEngine::GetMainCamera() {
	for (TActorIterator<ACameraActor> it(GetWorld()); it; ++it) {
		ACameraActor* Camera = *it;
		if (Camera && Camera->GetName().Contains("MainCamera")) {
			MainCam = Camera;
			PRINTTRAIN(TEXT("MainCam: %s"), *MainCam->GetName());
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Camera Fail"));
}

void ATrainEngine::AddTrainSpeed(float DeltaTime) {
	TrainSpeed += 0.03f * DeltaTime;

	for (int i = 1; i < TrainModules.Num(); ++i)
		TrainModules[i]->ModuleSpeed = TrainSpeed;
}

void ATrainEngine::AccelModules(float DeltaTime) {
	// 4초에 걸쳐서 가속
	EasingAlpha += DeltaTime / 4;

	TrainSpeed = FMath::Lerp(MinSpeed, MaxSpeed, FMath::Clamp((FMath::InterpEaseIn(0.0f, 1.0f, EasingAlpha, 2.0f)), 0.0f, 1.0f));

	for (int i = 1; i < TrainModules.Num(); ++i)
		TrainModules[i]->ModuleSpeed = TrainSpeed;
}

void ATrainEngine::DecelModules(float DeltaTime) {
	// 1초에 걸쳐서 감속
	EasingAlpha += DeltaTime;
	EasingAlpha = FMath::Clamp(EasingAlpha, 0.f, 1.f);

	TrainSpeed = FMath::Lerp(MaxSpeed, MinSpeed, FMath::Clamp((FMath::InterpEaseOut(0.0f, 1.0f, EasingAlpha, 2.0f)), 0.0f, 1.0f));

	for (int i = 1; i < TrainModules.Num(); ++i)
		TrainModules[i]->ModuleSpeed = TrainSpeed;
}

