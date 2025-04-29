// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "SBS/SBS_Player.h"
#include "../Express.h"
#include "ClearAnimWidget.h"
#include "SHS/SimpleUI.h"

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
	
	auto ClearAnim = CreateWidget<UClearAnimWidget>(this, ClearAnimFactory);
	ClearAnim->AddToViewport();
	ClearAnim->PlayClearAnimation();

	PRINTLOG(TEXT("Disabled Player Input"));
}

void ANetPlayerController::Client_ShowClearUI_Implementation() {
	auto player = Cast<ASBS_Player>(GetPawn());
	if (!player) return;

	auto ClearUI = CreateWidget<USimpleUI>(this, ClearUIFactory);
	ClearUI->bHasAuthority = HasAuthority();
	ClearUI->AddToViewport();

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
	PRINTLOG(TEXT("Show Clear UI"));
}

void ANetPlayerController::Client_ShowFailUI_Implementation() {
	auto player = Cast<ASBS_Player>(GetPawn());
	if (!player) return;

	auto FailUI = CreateWidget<USimpleUI>(this, FailUIFactory);
	FailUI->bHasAuthority = HasAuthority();
	FailUI->AddToViewport();
	
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
	PRINTLOG(TEXT("Show Failed UI"));
}
