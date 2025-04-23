// Fill out your copyright notice in the Description page of Project Settings.


#include "SHS/TrainWaterTank.h"
#include "Express/Express.h"
#include "SHS/TrainEngine.h"
#include "Exp_GameMode.h"
#include "Components/BoxComponent.h"
#include "SBS/SBS_Player.h"

ATrainWaterTank::ATrainWaterTank()
{
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

	if (!HasAuthority()) return;

	if (!bIsStarted) return;

	FireTimer += DeltaTime;

	ChangeTankColor();
}

void ATrainWaterTank::OnWaterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 일정 시간 이상 지나야만 물을 넣을 수 있음 (원작 고증)
	if (FireTimer <= 10.0f) return;

	ASBS_Player* player = Cast<ASBS_Player>(OtherActor);

	if (!player || !player->IsLocallyControlled()) return;

	if (player->HoldItems.IsEmpty() || player->HoldItems[0]->IsBucketEmpty) return;

	Server_EndFire(player);
}

void ATrainWaterTank::ChangeTankColor()
{
	if (!HasAuthority())
		PRINTFATALLOG(TEXT("Client Can't Use This Function."));

	MulticastRPC_ChangeTankColor();
}

void ATrainWaterTank::MulticastRPC_ChangeTankColor_Implementation() {
	float valueR = FireTimer / FireTime;
	float valueB = 1.0f - valueR;

	MaterialColor = FLinearColor(valueR, 0.0f, valueB);
	TankMaterial->SetVectorParameterValue(TEXT("TankColor"), MaterialColor);
}

