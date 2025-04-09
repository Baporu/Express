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

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetRelativeScale3D(FVector(1.0, 0.65, 0.8));

	ModuleComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ModuleComp"));
	ModuleComp->SetupAttachment(RootComponent);
	ModuleComp->SetBoxExtent(FVector(50.0));
	ModuleComp->SetRelativeLocation(FVector(-100.0, 0.0, 0.0));

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

	// �ð� �� ä��� ȭ�� ����
	if (FireTimer > FireTime) StartFire();

	// �� ���� ���¸� ȭ�� Ȯ�� ����
	if (bOnFire) OnFire(DeltaTime);
}

void ATrainModule::Init(ATrainEngine* EngineModule)
{
	TrainEngine = EngineModule;
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

void ATrainModule::OnFire(float DeltaTime)
{
	// �ڿ� ����� �ִ��� Ȯ��
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->TrainModules[ModuleNumber + 1]->FireTimer += DeltaTime;

	// ������ �����ϸ� �տ� ����� ���� �� ����
	TrainEngine->TrainModules[ModuleNumber - 1]->FireTimer += DeltaTime;
}

