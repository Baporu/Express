// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainModule.h"
#include "Components/BoxComponent.h"
#include "SHS/TrainEngine.h"
#include "Express/Express.h"
#include "Particles/ParticleSystemComponent.h"
#include "SBS/SBS_Player.h"
#include "Net/UnrealNetwork.h"

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

	FireComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireComp"));
	FireComp->SetupAttachment(RootComponent);
	FireComp->SetAutoActivate(false);

	WaterComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterComp"));
	WaterComp->SetupAttachment(RootComponent);
	WaterComp->SetBoxExtent(FVector(70.0, 50.0, 50.0));
	WaterComp->OnComponentBeginOverlap.AddDynamic(this, &ATrainModule::OnWaterBeginOverlap);

	bReplicates = true;
	SetReplicatingMovement(true);
}

// Called when the game starts or when spawned
void ATrainModule::BeginPlay()
{
	Super::BeginPlay();

	FireComp->SetTemplate(FireEffect);
}

// Called every frame
void ATrainModule::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// �������� ����
	if (!HasAuthority()) return;

	if (!bIsStarted) return;

	// �̵� �� ȸ�� ���� ����
	MoveTrain(DeltaTime);
	RotateTrain(DeltaTime);

	// ���� Ÿ�Ͽ� �������� ���
	if (FVector::Dist2D(GetActorLocation(), NextPos) <= 0.5) {
		// Queue�� �ִ� ���� ��ġ�� �̵�
		if (!TileQueue.IsEmpty())
			TileQueue.Dequeue(NextPos);
		// Queue�� �ִ� ���� ȸ�� �� ����
		if (!RotatorQueue.IsEmpty())
			RotatorQueue.Dequeue(NextRot);
	}

	if (bIsFinished) return;

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
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	// �̹� �� ���� ���¸� return
	if (bOnFire) return;

	MulticastRPC_StartFire();
}

void ATrainModule::EndFire()
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	MulticastRPC_EndFire();
	FireTimer = 0.0f;
}

void ATrainModule::MulticastRPC_StartFire_Implementation() {
	bOnFire = true;

	// ȭ�� ����Ʈ ��ȯ
	FireComp->Activate(true);
}

void ATrainModule::MulticastRPC_EndFire_Implementation() {
	FireComp->Deactivate();

	bOnFire = false;
}

void ATrainModule::SetModuleLocation(FVector CurrentLocation)
{
	if (TrainEngine->CheckModule(ModuleNumber + 1))
		TrainEngine->GetBackModule(ModuleNumber)->SetModuleLocation(NextPos);

	TileQueue.Enqueue(CurrentLocation);

//	UE_LOG(LogTrain, Log, TEXT("%s: Set Next Location - X: %f, Y: %f, Z: %f"), *GetActorNameOrLabel(), NextPos.X, NextPos.Y, NextPos.Z);

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

void ATrainModule::Server_EndFire_Implementation(class ASBS_Player* player) {
	Multicast_ResetWater(player);

	EndFire();
}

void ATrainModule::MoveTrain(float DeltaTime)
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	FVector dir = NextPos - GetActorLocation();
	FVector vt = dir.GetSafeNormal() * ModuleSpeed * DeltaTime;
	SetActorLocation(GetActorLocation() + vt);
}

void ATrainModule::RotateTrain(float DeltaTime)
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	double curRot = GetActorRotation().Yaw;
	curRot = FMath::Lerp(curRot, NextRot, DeltaTime);

	SetActorRotation(FRotator(0.0, curRot, 0.0));
}

void ATrainModule::OnFire(float DeltaTime)
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

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

	if (!player || !player->IsLocallyControlled()) return;

	if (player->HoldItems.IsEmpty() || player->HoldItems[0]->IsBucketEmpty) return;

	player->Server_RequestEndFire(this);
}

void ATrainModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATrainModule, FireTimer);
	DOREPLIFETIME(ATrainModule, TrainEngine);
	DOREPLIFETIME(ATrainModule, ModuleNumber);
	DOREPLIFETIME(ATrainModule, bIsFinished);
}

void ATrainModule::Multicast_ResetWater_Implementation(class ASBS_Player* player) {
	player->HoldItems[0]->IsBucketEmpty = true;
	player->HoldItems[0]->UpdateMeshMat();
}

