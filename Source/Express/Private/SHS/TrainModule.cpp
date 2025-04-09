// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainModule.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainEngine.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATrainModule::ATrainModule()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	ModuleComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ModuleComp"));
	ModuleComp->SetupAttachment(RootComponent);
	ModuleComp->SetBoxExtent(FVector(50.0));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded()) MeshComp->SetStaticMesh(tempMesh.Object);
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

	// �� ���� ���¸� ȭ�� Ȯ�� ����
	if (bOnFire) OnFire(DeltaTime);
}

void ATrainModule::AttachModule(ATrainModule* TrainModule)
{
	// ��� �ٲ㳢�� �� �����ϴ� ��
// 	int32 ModuleIndex = ModuleNumber + 1;
// 
// 	// �̹� �� �ڿ� ����� ������
// 	if (TrainEngine->TrainModules[ModuleIndex]) {
// 
// 	}

	TrainModule->AttachToComponent(ModuleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ATrainModule::StartFire()
{
	bOnFire = true;

	// ȭ�� ����Ʈ ��ȯ
	FireComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, GetActorTransform());
}

void ATrainModule::OnFire(float DeltaTime)
{
	// �ڿ� ����� �ִ��� Ȯ��
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;

	// ������ �����ϸ� �տ� ����� ���� �� ����
	TrainEngine->TrainModules[ModuleNumber - 1]->FireTimer += DeltaTime;
}

void ATrainModule::EndFire()
{
	FireComp->Deactivate();

	bOnFire = false;
	FireTimer = 0.0f;
}

