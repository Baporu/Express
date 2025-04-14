// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainModule.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainEngine.h"
#include "Express/Express.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

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

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded()) {
		MeshComp->SetStaticMesh(tempMesh.Object);
		ChainComp->SetStaticMesh(tempMesh.Object);
	}
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
	// 현재 모듈이 가던 타일의 위치를 다음 모듈로 넘겨주고
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->GetBackModule(ModuleNumber)->SetModuleLocation(NextPos);
	
	// 이전 모듈이 넘겨준 타일의 위치를 다음 목적지로 설정
	NextPos = CurrentLocation;
}

void ATrainModule::SetModuleRotation(double CurrentYaw)
{
	// 현재 모듈의 회전값을 다음 모듈로 넘겨주고
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->GetBackModule(ModuleNumber)->SetModuleRotation(NextRot);
	
	// 이전 모듈이 넘겨준 회전값 받기
	NextRot = CurrentYaw;
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

