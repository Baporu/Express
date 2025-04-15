// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainWaterTank.h"
#include "Express/Express.h"
#include "SHS/TrainEngine.h"
#include "Exp_GameMode.h"
#include "Components/BoxComponent.h"
#include "SBS/SBS_Player.h"

ATrainWaterTank::ATrainWaterTank()
{
	IAComp = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionComp"));
	IAComp->SetupAttachment(RootComponent);
	IAComp->SetBoxExtent(FVector(70.0, 50.0, 50.0));
	IAComp->OnComponentBeginOverlap.AddDynamic(this, &ATrainWaterTank::OnPlayerBeginOverlap);

	ConstructorHelpers::FObjectFinder<UMaterial> tempMat(TEXT("/Script/Engine.Material'/Game/SHS/Designs/M_WaterTank.M_WaterTank'"));
	if (tempMat.Succeeded()) MeshComp->SetMaterial(0, tempMat.Object);
}

// Called when the game starts or when spawned
void ATrainWaterTank::BeginPlay()
{
	Super::BeginPlay();

	TankMaterial = MeshComp->CreateDynamicMaterialInstance(0);

	AExp_GameMode* gm = Cast<AExp_GameMode>(GetWorld()->GetAuthGameMode());
	if (gm) gm->WaterTank = this;

	if (!TrainEngine) {
		UE_LOG(LogTrain, Log, TEXT("Train Engine Not Found"));

		return;
	}

	TrainEngine->AddFireTime(FireTime);
}

// Called every frame
void ATrainWaterTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTimer += DeltaTime;

	ChangeTankColor();
}

void ATrainWaterTank::EndFire()
{
	if (bOnFire) {
		Super::EndFire();

		return;
	}

	FireTimer = 0.0f;
}

void ATrainWaterTank::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 일정 시간 이상 지나야만 물을 넣을 수 있음 (원작 고증)
	if (FireTimer <= 10.0f) return;

	ASBS_Player* player = Cast<ASBS_Player>(OtherActor);

	if (!player || !player->bHasWater) return;

	player->bHasWater = false;
	EndFire();
}

void ATrainWaterTank::ChangeTankColor()
{
	float valueR = FireTimer / FireTime;
	float valueB = 1.0f - valueR;

	MaterialColor = FLinearColor(valueR, 0.0f, valueB);
	TankMaterial->SetVectorParameterValue(TEXT("TankColor"), MaterialColor);
}

