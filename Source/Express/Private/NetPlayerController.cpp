// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "SBS/SBS_Player.h"
#include "../Express.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bAutoManageActiveCameraTarget = false;
	SetMainCamera();

	if (HasAuthority()) //서버에서 호출
	{
		Server_SetMainCamera();
	}
}

void ANetPlayerController::SetMainCamera()
{
	for (TActorIterator<ACameraActor> it(GetWorld()); it; ++it)
	{
		ACameraActor* Camera = *it;
		if (Camera && Camera->GetName().Contains("MainCamera"))
		{
			SetViewTarget(Camera);
			UE_LOG(LogTemp, Log, TEXT("Set ViewTarget to %s"), *Camera->GetName());
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Camera Fail"));
}

void ANetPlayerController::Server_SetMainCamera_Implementation()
{
	SetMainCamera();
}

void ANetPlayerController::Client_DisableInput_Implementation() {
	auto player = Cast<ASBS_Player>(GetPawn());
	if (!player) return;

	player->bIsEnded = true;
	PRINTLOG(TEXT("Disabled Player Input"));
}
